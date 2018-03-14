<?hh // partial

/*
 * This file is part of the php-code-coverage package.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace SebastianBergmann\CodeCoverage\Driver;

use SebastianBergmann\CodeCoverage\Driver\HHVM\LineStack;
use SebastianBergmann\CodeCoverage\Driver\HHVM\CodeBlock\AbstractBlock;
use SebastianBergmann\CodeCoverage\Driver\HHVM\CodeBlock\CodeBlockInterface;
use SebastianBergmann\CodeCoverage\Driver\HHVM\CodeBlock\IfBlock;
use SebastianBergmann\CodeCoverage\Driver\HHVM\CodeBlock\ReturnBlock;
use SebastianBergmann\CodeCoverage\Driver\HHVM\CodeBlock\UseBlock;
use SebastianBergmann\CodeCoverage\Driver\HHVM\CodeBlock\ThrowBlock;
use SebastianBergmann\CodeCoverage\Driver\HHVM\CodeBlock\FunctionBlock;
use SebastianBergmann\CodeCoverage\Driver\HHVM\CodeBlock\MultiLineStatementBlock;

/**
 * Driver for HHVM's code coverage functionality.
 *
 * @codeCoverageIgnore
 */
class HHVM extends Xdebug {
  private array $_seenFile;
  private array $_execRanges;
  private bool $_debug;
  private $_codeBlocks;

  public function __construct() {
    $this->_seenFile = array();
    $this->_execRanges = array();
    $this->_debug = false;
    $this->_codeBlocks = null;
    parent::__construct();
  }

  /**
   * Start collection of code coverage information.
   *
   * @param bool $determineUnusedAndDead
   */
  public function start($determineUnusedAndDead = true) {

    //echo "XDEBUG-start called\n";
    //var_dump(get_included_files());
    xdebug_start_code_coverage();
  }

  /**
   * Stop collection of code coverage information.
   *
   * @return array
   */
  public function stop()
  {

    //echo "XDEBUG-stop called\n";
    // var_dump(get_included_files());

    $data = parent::stop();

    // --
    // JEO: We overload the stop function to enable us to fill in the data that
    //   is currently missing within hhvm's xdebug.
    // --
    // --
    // If there is more than one executionary pass, mark it as ran.
    // --
    $files = array_keys($data);

    foreach ($files as $file) {

      // only process real files.
      if ( ! file_exists($file) ) {
        continue;
      }

      // exclude any files that aren't hack related.
      // --
      // JEO: This is due to our inclusion chain within legacy php code, and how
      // we track results within this function.
      // --
      if ( ! preg_match('/.hh$/', $file ) ) {
        unset($data[$file]);
        continue;
      }

      $this->debugTokenCode("file=$file");

      if ( in_array($file, $this->_seenFile) !== true ) {

        $fileStack = $this->patchExecutableCodeIssue($file);

        foreach ( $fileStack as $line => $value ) {
          if ( ! isset($data[$file][$line]) ) {
            $data[$file][$line] = $value;
          }
        }

        $this->_seenFile[] = $file;

      }

      // --
      // HHVM xdebug reports the number of times something is executed,
      //  whereas php:xdebug just does a 0/1 state.
      // --
      $execStack = $data[$file];

      $didChange = $this->patchExecutedCodeIssue($file, $execStack);

      $data[$file] = $execStack;

    }

    // try to clean up after ourselves.
    // gc_collect_cycles();
    // --
    // JEO: ashwin discovered this is a very very expensive
    // call on newer versions of hhvm.
    // --

    return $data;

  }

  public function debugTokenCode($message): void {
    if ( $this->_debug === true ) {
      echo $message . "\n";
    }
  }

  public function patchExecutedCodeIssue(string $file, array $fileStack): bool {

    //echo "patchExcCode file=$file pre-fileStack\n";
    //ksort(&$fileStack);
    //var_dump($fileStack);

    $didChange = false;

    $fileStackKeys = array_keys($fileStack);

    // walk through the keys normalizing for hhvm's exec count change to xdebug.
    foreach ($fileStackKeys as $line ) {
      if ( $fileStack[$line] > 1 ) {
        $fileStack[$line] = Driver::LINE_EXECUTED;
        $didChange = true;
      }
    }

    // walk through the executable code ranges and mark the entire block as
    // executed -if- inners executed.
    foreach ( $this->_execRanges[$file] as $block ) {

      $startBlock = $block->getStartBlock();
      $endBlock = $block->getEndBlock();

      $isFunctionBlock = false;
      if ( $block instanceof FunctionBlock ) {
        $isFunctionBlock = true;
      }

      $this->debugTokenCode('EXEC_POSSIBLE_RANGE file=' . $file . ' block=' . get_class($block) . ' start=' . $startBlock . ' end=' . $endBlock);

      $didExecBlock = false;
      for ( $line = $startBlock; $line <= $endBlock; $line++ ) {
        if ( isset($fileStack[$line]) && $fileStack[$line] == Driver::LINE_EXECUTED ) {
          $didExecBlock = true;
        }
      }

      if ( $didExecBlock === true ) {

        for ( $line = $startBlock; $line <= $endBlock; $line++ ) {
          $fileStack[$line] = Driver::LINE_EXECUTED;
        }

        $this->debugTokenCode('COVERED_RANGE file=' . $file . ' start=' . $startBlock . ' end=' . $endBlock);
        $didChange = true;

      }


    }

    //echo "patchExcCode file=$file post-fileStack\n";
    //ksort(&$fileStack);
    //var_dump($fileStack);

    return $didChange;

  }

  public function enableDebug() {
    $this->_debug = true;
  }

  public function disableDebug() {
    $this->_debug = false;
  }

  public function processCodeBlock(CodeBlockInterface $block, string $file, LineStack $lineStack, int $currentLine, array $fileStack): bool {

    $block->isStartOfBlock($lineStack, $currentLine);

    // Have we found the end of the block we are hunting for?
    if ( $block->isEndOfBlock($lineStack, $currentLine) === true ) {

      $this->_execRanges[$file][] = $block;

      $isFileInclusion = $lineStack->isFileInclusion();

      if ( $isFileInclusion === true ) {
        $this->debugTokenCode(
          "file=$file block=" . get_class($block) . " rangeStart=" . $block->getStartBlock() .
          " rangeEnd=" . $block->getEndBlock() . " isFileInclusion=" . $lineStack->isFileInclusion()
        );
      }

      for ( $lineNo = $block->getStartBlock(); $lineNo <= $block->getEndBlock(); $lineNo++ ) {
        if ( ! isset($fileStack[$lineNo]) ) {
          $fileStack[$lineNo] = Driver::LINE_NOT_EXECUTED;
        }
        if ( $isFileInclusion === true ) {
          $fileStack[$lineNo] = Driver::LINE_EXECUTED;
        }
      }

      // JEO: This was returning true all the time, which would indicate we are
      // still within block afaik we've hit end of block due to this flow.
      return false;

    }

    // skip because we're within a multiline block
    if ( $block->getInBlock() === true ) {
      return true;
    }

    // let normal execution happen
    return false;

  }

  public function processCodeBlocks(string $file, LineStack $lineStack, int $currentLine, array $fileStack): bool {

    $codeBlocks = $this->getCodeBlocks();

    $this->debugTokenCode("processCodeBlocks currentLine=" . $currentLine);

    foreach ( $codeBlocks as $codeBlock ) {
      //echo "codeBlock=" . get_class($codeBlock) . "\n";
      if ( $this->processCodeBlock($codeBlock, $file, $lineStack, $currentLine, $fileStack) === true ) {
        // Advance the outer line processor loop, until the code block is completed.
        $this->debugTokenCode('  PROCESS_CODE_BLOCKS - true, block found currentLine=' . $currentLine);
        return true;
      }
    }

    $this->debugTokenCode('  PROCESS_CODE_BLOCKS - false, block not present currentLine=' . $currentLine);
    return false;

  }

  public function initCodeBlocks() {

    if ( $this->_codeBlocks === null ) {
      $this->_codeBlocks = Vector {};
    } else {
      $this->_codeBlocks->clear();
    }

    $this->_codeBlocks[] = new AbstractBlock();
    $this->_codeBlocks[] = new IfBlock();
    $this->_codeBlocks[] = new ReturnBlock();
    $this->_codeBlocks[] = new UseBlock();
    $this->_codeBlocks[] = new ThrowBlock();
    // $this->_codeBlocks[] = new FunctionBlock();
    $this->_codeBlocks[] = new MultiLineStatementBlock();

    return true;
  }

  public function getCodeBlocks() {

    if ( $this->_codeBlocks === null ) {
      $this->initCodeBlocks();
    }

    return $this->_codeBlocks;

  }

  public function resetCodeBlocks() {

    $this->initCodeBlocks();
    /*
    $codeBlocks = $this->getCodeBlocks();

    foreach ( $this->_codeBlocks as $codeBlock ) {
      $codeBlock->reset();
    }
    */

  }

  public function patchExecutableCodeIssue($file) {

    $fileStack = array();

    $this->_execRanges[$file] = array();

    // --
    // JEO: you might want to turn off this caching if you don't run a significant
    // amount of ram within your machines.
    // --
    $tokens = \PHP_Token_Stream_CachingFactory::get($file);

    // Loop across the tokens
    $lineStack = new LineStack();

    $inCodeBlock = false;
    $codeBlockStart = 0;

    $currentLine = 0;
    $tokens_cnt = count($tokens);
    for ( $tokenOffset = 0; $tokenOffset < $tokens_cnt; $tokenOffset++) {

      $token = $tokens[$tokenOffset];

      // line number for the token.
      $line = $token->getLine();

      // Has the lineno changed?
      if (  $currentLine != $line ) {

        $this->debugTokenCode("currentLine=$currentLine != $line");

        // We are done processing the code block if we get a false back from processCodeBlocks
        if ( $this->processCodeBlocks($file, $lineStack, $currentLine, $fileStack) === true ) {

          $this->debugTokenCode("  PROCESS_CODE_BLOCKS ret=true line=" . $currentLine . " tokens=" . $lineStack->toJSON() . ' text=' . $lineStack->getLineText() );

          // advance the line to new item.
          $currentLine = $line;

          // Get the class name for this token.
          $tokenType = get_class($token);

          // add to line stack
          $lineStack->addToken($tokenType);
          $lineStack->addLineText(strval($token));


          continue;

        }

        $isExecutableLine = $lineStack->isExecutable();
        $doesContainSemiColon = $lineStack->doesContainSemiColon();

        $this->debugTokenCode("isExecutableLine=$isExecutableLine doesContainSemiColon=$doesContainSemiColon");

        if ( $currentLine !== 0 && $isExecutableLine === true ) {

          if ( ! isset($fileStack[$currentLine]) ) {
            $this->debugTokenCode("  LIVE_CODE_NOT_EXECUTED file=$file line=" . $currentLine . " tokens=" . $lineStack->toJSON() . ' text=' . $lineStack->getLineText() );
            $fileStack[$currentLine] = Driver::LINE_NOT_EXECUTED;
          }

        } else {
          $this->debugTokenCode("  NOT_CODE_NOT_EXECUTED line=" . $currentLine . " tokens=" . $lineStack->toJSON() . ' text=' . $lineStack->getLineText());
        }

        // clear stack
        $lineStack->clear();
        $this->resetCodeBlocks();

        // advance the line to new item.
        $currentLine = $line;

      }

      // Get the class name for this token.
      $tokenType = get_class($token);

      // add to line stack
      $lineStack->addToken($tokenType);
      $lineStack->addLineText(strval($token));

    }

    return $fileStack;

  }

}
