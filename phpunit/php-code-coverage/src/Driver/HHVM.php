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
    gc_collect_cycles();

    return $data;

  }

  public function debugTokenCode($message) {
    if ( $this->_debug === true ) {
      echo $message . "\n";
    }
  }

  public function patchExecutedCodeIssue(string $file, array $fileStack): (bool, array) {

    $didChange = false;

    // walk through the keys normalizing for hhvm's exec count change to xdebug.
    foreach (array_keys($fileStack) as $line ) {
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

  public function isLineStackExecutable($lineStack) {

    $fileInclusion = array();
    $fileInclusion[] = 'PHP_Token_REQUIRE';
    $fileInclusion[] = 'PHP_Token_REQUIRE_ONCE';
    $fileInclusion[] = 'PHP_Token_INCLUDE';
    $fileInclusion[] = 'PHP_Token_INCLUDE_ONCE';

    foreach ( $fileInclusion as $fileInclude ) {
      if ( in_array($fileInclude, $lineStack) ) {
        return false;
      }
    }

    // --
    // If there is a parseable / executable token this counts as a line that 'could' be executed.
    // --
    $executableTokens = array();
    $executableTokens[] = 'PHP_Token_RETURN';
    $executableTokens[] = 'PHP_Token_THROW';
    $executableTokens[] = 'PHP_Token_EQUAL';
    $executableTokens[] = 'PHP_Token_IF';

    // $executableTokens[] = 'PHP_Token_FOREACH';
    // --
    // $executableTokens[] = 'PHP_Token_VARIABLE'; -- this grabs too many lines of code.
    // --
    $executableTokens[] = 'PHP_Token_OBJECT_OPERATOR';
    $executableTokens[] = 'PHP_Token_SEMICOLON';

    foreach ( $lineStack as $token ) {
      if ( in_array($token, $executableTokens) === true ) {
        return true;
      }
    }

    return false;

  }

  public function doesLineContainSemiColon($lineStack) {
    if ( in_array('PHP_Token_SEMICOLON', $lineStack) ) {
      return true;
    }
    return false;
  }


  public function doesLineContainIf($lineStack) {
    if ( in_array('PHP_Token_IF', $lineStack) ) {
      return true;
    }
    return false;
  }

  public function enableDebug() {
    $this->_debug = true;
  }

  public function disableDebug() {
    $this->_debug = false;
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
    $lineStack = array();
    $lineText = '';

    $inCodeBlock = false;
    $codeBlockStart = 0;

    $currentLine = 0;
    for ( $tokenOffset = 0; $tokenOffset < count($tokens); $tokenOffset++) {
      $token = $tokens[$tokenOffset];

      // line number for the token.
      $line = $token->getLine();

      // Has the lineno changed?
      if (  $currentLine != $line ) {

        // do processing logic.
        if ( $currentLine !== 0 && $this->isLineStackExecutable($lineStack) === true ) {
          $this->debugTokenCode("  LIVE_CODE_NOT_EXECUTED line=" . $currentLine . " tokens=" . json_encode($lineStack) . ' text=' . $lineText);
          $fileStack[$currentLine] = Driver::LINE_NOT_EXECUTED;

          // Does this executable line contain a semi colon?
          if ( $inCodeBlock !== true && $this->doesLineContainSemiColon($lineStack) !== true && $this->doesLineContainIf($lineStack) !== true ) {
            $inCodeBlock = true;
            $codeBlockStart = $currentLine;
            $this->debugTokenCode('  START_RANGE start=' . $codeBlockStart);
          } else if ( $inCodeBlock === true && $this->doesLineContainSemiColon($lineStack) === true ) {
            // end block
            $codeBlockEnd = $currentLine;

            $this->debugTokenCode('  EXEC_RANGE start=' . $codeBlockStart . ' end=' . $codeBlockEnd);

            $this->_execRanges[$file][] = array($codeBlockStart, $codeBlockEnd);

            $inCodeBlock = false;
          }
        } else if ( $inCodeBlock === true && $this->doesLineContainSemiColon($lineStack) === true ) {

          // end block
          $codeBlockEnd = $currentLine;

          $this->debugTokenCode('  OUT_RANGE start=' . $codeBlockStart . ' end=' . $codeBlockEnd);

          $this->_execRanges[$file][] = array($codeBlockStart, $codeBlockEnd);

          $inCodeBlock = false;

        } else {
          // $this->debugTokenCode("  NOT_CODE_NOT_EXECUTED line=" . $currentLine . " tokens=" . json_encode($lineStack) . ' text=' . $lineText);
        }

        // clear stack
        $lineStack = array();
        $lineText = '';

        // advance the line to new item.
        $currentLine = $line;

      }

      // Get the class name for this token.
      $tokenType = get_class($token);

      // add to line stack
      $lineStack[] = $tokenType;

      $lineText .= $token;

    }

    return $fileStack;

  }

}
