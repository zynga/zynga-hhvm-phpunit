<?hh // strict

namespace Zynga\PHPUnit\V2\Tests\System;

use Zynga\Framework\Environment\CodePath\V1\CodePath;
use Zynga\PHPUnit\V2\TestFailure;
use Zynga\PHPUnit\V2\TestCase;
use Zynga\PHPUnit\V2\TestCase\Status;
use Zynga\PHPUnit\V2\TestResult;
use Zynga\PHPUnit\V2\TestSuite;
use \Exception;

abstract class BaseTest extends TestCase {

  public function getTestSourcePath(): string {
    return dirname(__FILE__);

    // CodePath::getRoot().
    // DIRECTORY_SEPARATOR.
    // 'src'.
    // DIRECTORY_SEPARATOR.
    // 'Zynga'.
    // DIRECTORY_SEPARATOR.
    // 'PHPUnit'.
    // DIRECTORY_SEPARATOR.
    // 'V2'.
    // DIRECTORY_SEPARATOR.
    // 'Tests'.
    // DIRECTORY_SEPARATOR.
    // 'Mock'.
    // DIRECTORY_SEPARATOR;
  }

  protected function _debugException(Exception $e): void {

    print 'message='."\n";
    var_dump($e->getMessage());
    print 'file='.$e->getFile().'('.$e->getLine().')'."\n";
    print 'trace='."\n";
    $offset = 0;
    foreach ($e->getTrace() as $traceFrame) {
      $file = '';
      $class = '';
      $function = '';
      $line = '';

      if (array_key_exists('file', $traceFrame)) {
        $file = $traceFrame['file'];
      }
      if (array_key_exists('class', $traceFrame)) {
        $class = $traceFrame['class'].'::';
      }
      if (array_key_exists('function', $traceFrame)) {
        $function = $traceFrame['function'];
      }

      if (array_key_exists('line', $traceFrame)) {
        $line = $traceFrame['line'];
      }

      print 'frame='.$offset."\n";
      print '  file='.$file.'('.$line.') '.$class.$function."\n";

      $offset++;
    }
  }

  protected function _debugExceptions(Vector<TestFailure> $exceptions): void {
    foreach ($exceptions as $exception) {
      $this->_debugException($exception->thrownException());
    }
  }

  protected function _debugTestResult(
    TestResult $result,
    bool $debug,
    int $errorCount = 0,
    int $failureCount = 0,
    int $skippedCount = 0,
    int $incompleteCount = 0,
  ): void {

    if ($debug != true) {
      return;
    }

    $actualErrorCount = $result->errorCount();
    $actualErrors = $result->errors();

    print
      'errorCount expected='.$errorCount.' actual='.$actualErrorCount."\n"
    ;
    $this->_debugExceptions($actualErrors);

    $actualFailureCount = $result->failureCount();
    $actualFailures = $result->failures();

    print
      'failureCount'.
      ' expected='.
      $failureCount.
      ' actual='.
      $actualFailureCount.
      "\n"
    ;

    $this->_debugExceptions($actualFailures);

    $actualSkippedCount = $result->skippedCount();
    $actualSkipped = $result->skipped();

    print
      'skippedCount'.
      ' expected='.
      $skippedCount.
      ' actual='.
      $actualSkippedCount.
      "\n"
    ;

    $this->_debugExceptions($actualSkipped);

    $actualIncompleteCount = $result->notImplementedCount();
    $actualIncomplete = $result->notImplemented();

    print
      'incompleteCount'.
      'expected='.
      $incompleteCount.
      ' actual='.
      $actualIncompleteCount.
      "\n"
    ;

    $this->_debugExceptions($actualIncomplete);

  }

  protected function _verifyTestResult(
    TestResult $result,
    bool $debug = false,
    int $errorCount = 0,
    int $failureCount = 0,
    int $skippedCount = 0,
    int $incompleteCount = 0,
  ): void {

    $this->assertEquals($errorCount, $result->errorCount(), 'errorCount');

    $this->assertEquals(
      $failureCount,
      $result->failureCount(),
      'failureCount',
    );

    $this->assertEquals(
      $skippedCount,
      $result->skippedCount(),
      'skippedCount',
    );

    $this->assertEquals(
      $incompleteCount,
      $result->notImplementedCount(),
      'incompleteCount',
    );

  }

  protected function _verifyTestSuite(
    TestSuite $test,
    TestResult $result,
    bool $debug = false,
    int $statusCode = Status::STATUS_PASSED,
    int $testCount = 0,
    int $successfulCount = 0,
    int $errorCount = 0,
    int $failureCount = 0,
    int $skippedCount = 0,
    int $incompleteCount = 0,
    int $warningCount = 0,
    int $notImplemented = 0,
  ): void {

    if ($debug == true) {

      print
        "---verifyTestSuite::debug isEnabled for test=".
        $test->getName().
        "---\n"
      ;

      print
        "testCount"." expected=".$testCount.' actual='.$test->count()."\n"
      ;

      print
        "successfulCount".
        " expected=".
        $successfulCount.
        ' actual='.
        $result->successfulCount().
        "\n"
      ;

      print
        "warningCount".
        " expected=".
        $warningCount.
        ' actual='.
        $result->warningCount().
        "\n"
      ;

      print
        "notImplemented".
        " expected=".
        $notImplemented.
        ' actual='.
        $result->notImplementedCount().
        "\n"
      ;

    }

    $this->assertEquals($testCount, $test->count(), 'testCount');

    $this->assertEquals(
      $successfulCount,
      $result->successfulCount(),
      'successfulCount',
    );

    $this->assertEquals(
      $warningCount,
      $result->warningCount(),
      'warningCount',
    );

    $this->assertEquals($notImplemented, $result->notImplementedCount());

    $this->_debugTestResult(
      $result,
      $debug,
      $errorCount,
      $failureCount,
      $skippedCount,
      $incompleteCount,
    );

    $this->_verifyTestResult(
      $result,
      $debug,
      $errorCount,
      $failureCount,
      $skippedCount,
      $incompleteCount,
    );

  }

  protected function _verifyTest(
    TestCase $test,
    TestResult $result,
    bool $debug = false,
    int $statusCode = Status::STATUS_PASSED,
    string $statusMessage = '',
    int $errorCount = 0,
    int $failureCount = 0,
    int $skippedCount = 0,
    int $incompleteCount = 0,
  ): void {

    if ($debug == true) {

      // --
      // JEO: Need a better way to dump things into phphunit's output buffer.
      // --
      var_dump('_verifyTestResults::debug');

      var_dump(
        'statusCode expected='.$statusCode.' actual='.$test->getStatus(),
      );

      var_dump('statusMessage expected=');
      var_dump($statusMessage);

      var_dump('statusMessage actual=');
      var_dump($test->getStatusMessage());

      print 'expectedOutput='.$test->getExpectedOutput()."\n";
      print 'expectedOutputRegex='.$test->getExpectedOutputRegex()."\n";

      print 'expectedException='.$test->getExpectedException()."\n";

      print
        'expectedExceptionMessage='.
        $test->getExpectedExceptionMessage().
        "\n"
      ;

      print
        'expectedExceptionMessageRegExp='.
        $test->getExpectedExceptionMessageRegExp().
        "\n"
      ;

      $this->_debugTestResult(
        $result,
        $debug,
        $errorCount,
        $failureCount,
        $skippedCount,
        $incompleteCount,
      );

    }

    $this->assertEquals($statusCode, $test->getStatus());
    $this->assertEquals($statusMessage, $test->getStatusMessage());

    $this->_verifyTestResult(
      $result,
      $debug,
      $errorCount,
      $failureCount,
      $skippedCount,
      $incompleteCount,
    );

  }

}
