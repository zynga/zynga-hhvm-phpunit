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

/**
 * Driver for HHVM's code coverage functionality.
 *
 * @codeCoverageIgnore
 */
class HHVM extends Xdebug
{

  private array $_seenFile;
  private array $_execRanges;
  private bool $_debug;

  public function __construct() {
    $this->_seenFile = array();
    $this->_execRanges = array();
    $this->_debug = false;
    parent::__construct();
  }

  /**
   * Start collection of code coverage information.
   *
   * @param bool $determineUnusedAndDead
   */
  public function start($determineUnusedAndDead = true)
  {
    xdebug_start_code_coverage();
  }

  public function withinAcceptableCodePath($file): bool {
    if ( preg_match('/^\/(usr|opt)/', $file) ) {
      return false;
    }
    return true;
  }
  /**
   * Stop collection of code coverage information.
   *
   * @return array
   */
  public function stop()
  {

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
      // A different strategy, exclude /opt/
      // if ( $this->withinAcceptableCodePath($file) === false ) {
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

      list($didChange, $execStack) = $this->patchExecutedCodeIssue($file, $execStack);

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

  public function debugTokenCode($message) {
    if ( $this->_debug === true ) {
      echo $message . "\n";
    }
  }

  public function patchExecutedCodeIssue(string $file, array $fileStack): (bool, array) {

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

      list($startBlock, $endBlock) = $block;


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

    /*
    var_dump($file);
    var_dump($fileStack);
    */

    return tuple($didChange, $fileStack);

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

      $this->_execRanges[$file][] = array( $block->getStartBlock(), $block->getEndBlock() );

      for ( $lineNo = $block->getStartBlock(); $lineNo <= $block->getEndBlock(); $lineNo++ ) {
        $fileStack[$lineNo] = Driver::LINE_NOT_EXECUTED;
      }

      return true;

    }

    // skip because we're within a if
    if ( $block->getInBlock() === true ) {
      return true;
    }

    // let normal execution happen
    return false;

  }

  public function processCodeBlocks(Vector<CodeBlockInterface> $codeBlocks, string $file, LineStack $lineStack, int $currentLine, array $fileStack): bool {

    foreach ( $codeBlocks as $codeBlock ) {
      if ( $this->processCodeBlock($codeBlock, $file, $lineStack, $currentLine, $fileStack) === true ) {
        // Advance the outer line processor loop, until the code block is completed.
        return true;
      }
    }

    return false;

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

    $codeBlocks = Vector {};
    $codeBlocks[] = new AbstractBlock();
    $codeBlocks[] = new IfBlock();
    $codeBlocks[] = new ReturnBlock();

    $inAbstractFunction = false;

    $currentLine = 0;
    $tokens_cnt = count($tokens);
    for ( $tokenOffset = 0; $tokenOffset < $tokens_cnt; $tokenOffset++) {

      $token = $tokens[$tokenOffset];

      // line number for the token.
      $line = $token->getLine();

      // Has the lineno changed?
      if (  $currentLine != $line ) {

        if ( $this->processCodeBlocks($codeBlocks, $file, $lineStack, $currentLine, $fileStack) === true ) {
          continue;
        }

        if ( $currentLine !== 0 && $lineStack->isExecutable() === true ) {

          $this->debugTokenCode("  LIVE_CODE_NOT_EXECUTED line=" . $currentLine . " tokens=" . $lineStack->toJSON() . ' text=' . $lineStack->getLineText() );
          $fileStack[$currentLine] = Driver::LINE_NOT_EXECUTED;

          // Does this executable line contain a semi colon?
          if ( $inCodeBlock !== true && $lineStack->doesContainSemiColon() !== true && $lineStack->doesContainIf() !== true ) {
            $inCodeBlock = true;
            $codeBlockStart = $currentLine;
            $this->debugTokenCode('  START_RANGE start=' . $codeBlockStart);
          } else if ( $inCodeBlock === true && $lineStack->doesContainSemiColon() === true ) {
            // end block
            $codeBlockEnd = $currentLine;

            $this->debugTokenCode('  EXEC_RANGE start=' . $codeBlockStart . ' end=' . $codeBlockEnd);

            $this->_execRanges[$file][] = array($codeBlockStart, $codeBlockEnd);

            $inCodeBlock = false;
          }
        } else if ( $inCodeBlock === true && $lineStack->doesContainSemiColon() === true ) {

          // end block
          $codeBlockEnd = $currentLine;

          $this->debugTokenCode('  OUT_RANGE start=' . $codeBlockStart . ' end=' . $codeBlockEnd);

          $this->_execRanges[$file][] = array($codeBlockStart, $codeBlockEnd);

          $inCodeBlock = false;

        } else {
          // $this->debugTokenCode("  NOT_CODE_NOT_EXECUTED line=" . $currentLine . " tokens=" . $lineStack->toJSON() . ' text=' . $lineStack->getLineText());
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

    return $fileStack;

  }

}
