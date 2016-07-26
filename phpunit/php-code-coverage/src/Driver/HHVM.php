<?php
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

  private array $_data;

  /**
   * Start collection of code coverage information.
   *
   * @param bool $determineUnusedAndDead
   */
  public function start($determineUnusedAndDead = true)
  {
    xdebug_start_code_coverage();
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

      if ( ! isset($this->_data[$file]) ) {
        // mark all code once as not-executed, we do this once for each file
        //  executed as part of your unit test run. There are cases where the
        //  the same file will get processed multiple times.
        // --
        // HHVM has no dead code analysis at this time. We are treating all
        //   execulatable code as live code and marking the code as such.
        //
        // We utilize the cacheTokens="true" functionality to enhance performance
        //   but it requires a higher memory heap for your unit tests under
        //   coverage.
        // --
        $fileStack = $this->patchExecutableCodeIssue($file);
        $this->_data[$file] = $fileStack;
      }

      $this->debugTokenCode("file=$file");

      // --
      // HHVM xdebug reports the number of times something is executed,
      //  whereas php:xdebug just does a 0/1 state.
      // --
      $fileStack = $this->patchExecutedCodeIssue($file, $data[$file]);

      // --
      // Overlay the overal data rep with the stack rep.
      // --
      foreach ( $fileStack as $line => $value ) {
        $this->_data[$file][$line] = $value;
      }

    }

    return $this->_data;

  }

  public function debugTokenCode($message) {
    if ( $this->_debug === true ) {
      echo $message . "\n";
    }
  }

  public function patchExecutedCodeIssue($file, $fileStack) {
    foreach (array_keys($fileStack) as $line ) {
      if ( $fileStack[$line] > 1 ) {
        $fileStack[$line] = Driver::LINE_EXECUTED;
      }
    }
    return $fileStack;
  }

  public function isLineStackExecutable($lineStack) {

    // --
    // If there is a parseable / executable token this counts as a line that 'could' be executed.
    // --
    $executableTokens = array();
    $executableTokens[] = 'PHP_Token_RETURN';
    $executableTokens[] = 'PHP_Token_THROW';
    $executableTokens[] = 'PHP_Token_EQUAL';
    $executableTokens[] = 'PHP_Token_IF';

    // $executableTokens[] = 'PHP_Token_FOREACH';
    // $executableTokens[] = 'PHP_Token_VARIABLE';
    $executableTokens[] = 'PHP_Token_OBJECT_OPERATOR';

    foreach ( $lineStack as $token ) {
      if ( in_array($token, $executableTokens) === true ) {
        return true;
      }
    }

    return false;

  }

  private bool $_debug = false;

  public function enableDebug() {
    $this->_debug = true;
  }

  public function disableDebug() {
    $this->_debug = false;
  }

  public function patchExecutableCodeIssue($file) {

    $fileStack = array();

    // --
    // JEO: you might want to turn off this caching if you don't run a significant
    // amount of ram within your machines.
    // --
    $tokens = \PHP_Token_Stream_CachingFactory::get($file);

    // Loop across the tokens
    $lineStack = array();

    $currentLine = 0;
    foreach ($tokens as $token) {

      // line number for the token.
      $line = $token->getLine();

      // Has the lineno changed?
      if (  $currentLine != $line ) {

        // do processing logic.
        if ( $currentLine !== 0 && $this->isLineStackExecutable($lineStack) === true ) {
          $this->debugTokenCode("LIVE_CODE_NOT_EXECUTED line=" . $currentLine . " tokens=" . json_encode($lineStack));
          $fileStack[$currentLine] = Driver::LINE_NOT_EXECUTED;
        }

        // clear stack
        $lineStack = array();

        // advance the line to new item.
        $currentLine = $line;

      }

      // Get the class name for this token.
      $tokenType = get_class($token);

      // add to line stack
      $lineStack[] = $tokenType;

    }

    return $fileStack;

  }

}
