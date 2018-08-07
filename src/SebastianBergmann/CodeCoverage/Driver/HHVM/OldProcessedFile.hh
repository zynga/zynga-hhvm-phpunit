<?hh // strict

namespace SebastianBergmann\CodeCoverage\Driver\HHVM;

use SebastianBergmann\CodeCoverage\Driver;
use SebastianBergmann\CodeCoverage\Driver\HHVM\ExecBlock;
use SebastianBergmann\CodeCoverage\Driver\HHVM\Logging as HHVM_Logging;
use SebastianBergmann\CodeCoverage\Driver\HHVM\CodeBlock\AbstractBlock;
use SebastianBergmann\CodeCoverage\Driver\HHVM\CodeBlock\CodeBlockInterface;
use SebastianBergmann\CodeCoverage\Driver\HHVM\CodeBlock\IfBlock;
use SebastianBergmann\CodeCoverage\Driver\HHVM\CodeBlock\ReturnBlock;
use
  SebastianBergmann\TokenStream\Token\Stream\CachingFactory as StreamCachingFactory
;

class OldProcessedFile {

  private string $_file;
  private Map<int, int> $_stack;
  private Vector<ExecBlock> $_execRanges;

  public function __construct(string $file) {
    $this->_file = $file;
    $this->_stack = Map {};
    $this->_execRanges = Vector {};
  }

  public function setStackValue(int $lineNo, int $lineState): void {

    $currentValue = $this->_stack->get($lineNo);

    if ($currentValue === null) {
      // no value at all for the stack.
      $this->_stack->set($lineNo, $lineState);
      return;
    }

    if ($lineState !== Driver::LINE_NOT_EXECUTABLE &&
        $lineState !== Driver::LINE_NOT_EXECUTED &&
        $lineState !== Driver::LINE_EXECUTED) {
      Logging::debug(
        'WARNING-INVALID-LINE_STATE lineNo='.
        $lineNo.
        ' lineState='.
        $lineState,
      );
      return;
    }

    if ($currentValue > $lineState) {
      error_log(
        'ATEMPTED_LINESTATE_DEGRADE-REFUSED '.
        'file='.
        $this->_file.
        ' '.
        'line='.
        $lineNo.
        ' '.
        'currentValue='.
        $currentValue.
        ' '.
        'lineState='.
        $lineState,
      );
      return;
    }

    $this->_stack->set($lineNo, $lineState);

  }

  public function initStackForFileName(): void {

    // --
    // JEO: you might want to turn off this caching if you don't run a significant
    // amount of ram within your machines.
    // --
    $tokens = StreamCachingFactory::get($this->_file);

    // Loop across the tokens
    $lineStack = new LineStack();

    $inCodeBlock = false;
    $codeBlockStart = 0;

    $codeBlocks = Vector {};
    $codeBlocks[] = new AbstractBlock();
    $codeBlocks[] = new IfBlock();
    $codeBlocks[] = new ReturnBlock();

    $inAbstractFunction = false;

    $currentLine = 0;
    $tokens_cnt = $tokens->count();
    for ($tokenOffset = 0; $tokenOffset < $tokens_cnt; $tokenOffset++) {

      $token = $tokens->get($tokenOffset);

      if ($token === null) {
        continue;
      }

      // line number for the token.
      $line = $token->getLine();

      // Has the lineno changed?
      if ($currentLine != $line) {

        if ($this->processCodeBlocks($codeBlocks, $lineStack, $currentLine) ===
            true) {
          continue;
        }

        $doesContainSemiColon = $lineStack->doesContainSemiColon();

        if ($currentLine !== 0 && $lineStack->isExecutable() === true) {

          HHVM_Logging::debug(
            "  LIVE_CODE_NOT_EXECUTED line=".
            $currentLine.
            " tokens=".
            $lineStack->toJSON().
            ' text='.
            $lineStack->getLineText(),
          );

          $this->setStackValue($currentLine, Driver::LINE_NOT_EXECUTED);

          // Does this executable line contain a semi colon?

          $doesContainIf = $lineStack->doesContainIf();

          if ($inCodeBlock !== true &&
              $doesContainSemiColon !== true &&
              $doesContainIf !== true) {
            $inCodeBlock = true;
            $codeBlockStart = $currentLine;
            HHVM_Logging::debug('  START_RANGE start='.$codeBlockStart);
          } else if ($inCodeBlock === true &&
                     $doesContainSemiColon === true) {
            // end block
            $codeBlockEnd = $currentLine;

            HHVM_Logging::debug(
              '  EXEC_RANGE start='.$codeBlockStart.' end='.$codeBlockEnd,
            );

            $this->addExecBlock($codeBlockStart, $codeBlockEnd);

            $inCodeBlock = false;
          }
        } else if ($inCodeBlock === true && $doesContainSemiColon === true) {

          // end block
          $codeBlockEnd = $currentLine;

          HHVM_Logging::debug(
            '  OUT_RANGE start='.$codeBlockStart.' end='.$codeBlockEnd,
          );

          $this->addExecBlock($codeBlockStart, $codeBlockEnd);

          $inCodeBlock = false;

        } else {
          HHVM_Logging::debug(
            "  NOT_CODE_NOT_EXECUTED line=".
            $currentLine.
            " tokens=".
            $lineStack->toJSON().
            ' text='.
            $lineStack->getLineText(),
          );
        }

        // clear stack
        $lineStack->clear();

        // advance the line to new item.
        $currentLine = $line;

      }

      // Get the class name for this token.
      $tokenType = get_class($token);

      // add to line stack
      $lineStack->addToken($tokenType);
      $lineStack->addLineText(strval($token));

    }

    foreach ($this->_execRanges as $block) {

      $startBlock = $block->getStart();
      $endBlock = $block->getStop();

      $didExecBlock = false;
      for ($line = $startBlock; $line <= $endBlock; $line++) {
        $this->setStackValue($line, Driver::LINE_NOT_EXECUTED);
      }

    }

  }

  public function processCodeBlocks(
    Vector<CodeBlockInterface> $codeBlocks,
    LineStack $lineStack,
    int $currentLine,
  ): bool {

    foreach ($codeBlocks as $codeBlock) {
      if ($this->processCodeBlock($codeBlock, $lineStack, $currentLine) ===
          true) {
        Logging::debug('  CODE_BLOCK_START='.get_class($codeBlock));
        // Advance the outer line processor loop, until the code block is completed.
        return true;
      }
    }

    return false;

  }

  public function processCodeBlock(
    CodeBlockInterface $block,
    LineStack $lineStack,
    int $currentLine,
  ): bool {

    $block->isStartOfBlock($lineStack, $currentLine);

    // Have we found the end of the block we are hunting for?
    if ($block->isEndOfBlock($lineStack, $currentLine) === true) {

      Logging::debug(
        '  CODE_BLOCK_END='.
        get_class($block).
        ' range='.
        $block->getStartBlock().
        ':'.
        $block->getEndBlock(),
      );

      $this->addExecBlock($block->getStartBlock(), $block->getEndBlock());

      for (
        $lineNo = $block->getStartBlock();
        $lineNo <= $block->getEndBlock();
        $lineNo++
      ) {
        $this->setStackValue($lineNo, Driver::LINE_NOT_EXECUTED);
      }

      return true;

    }

    // skip because we're within a if
    if ($block->getInBlock() === true) {
      return true;
    }

    // let normal execution happen
    return false;

  }

  public function addExecBlock(int $codeBlockStart, int $codeBlockEnd): void {

    $this->_execRanges->add(new ExecBlock($codeBlockStart, $codeBlockEnd));

  }

  public function toArrayFormat(): array<int, int> {
    return $this->_stack->toArray();
  }

  public function consumeRawExecStack(array<int, int> $fileStack): void {

    // walk through the keys normalizing for hhvm's exec count change to xdebug.
    foreach ($fileStack as $lineNo => $stateValue) {
      if ($stateValue >= 1) {
        $this->setStackValue($lineNo, Driver::LINE_EXECUTED);
      }
    }

    // walk through the executable code ranges and mark the entire block as
    // executed -if- inners executed.
    foreach ($this->_execRanges as $block) {

      $startBlock = $block->getStart();
      $endBlock = $block->getStop();

      $didExecBlock = false;
      for ($line = $startBlock; $line <= $endBlock; $line++) {

        if (array_key_exists($line, $fileStack) && $fileStack[$line] >= 1) {
          $didExecBlock = true;
        }
      }

      if ($didExecBlock === true) {
        for ($line = $startBlock; $line <= $endBlock; $line++) {
          HHVM_Logging::debug(
            'COVERED_RANGE file='.
            $this->_file.
            ' start='.
            $startBlock.
            ' end='.
            $endBlock.
            ' line='.
            $line,
          );
          $this->setStackValue($line, Driver::LINE_EXECUTED);
        }
      }

    }

  }

}
