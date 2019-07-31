<?hh // strict

namespace Zynga\PHPUnit\V2\Tests\Framework;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

//use SebastianBergmann\TextTemplate\Template as Text_Template;

use Zynga\Framework\Environment\SuperGlobals\V1\SuperGlobals;

use Zynga\PHPUnit\V2\Tests\BaseTest;

use Zynga\PHPUnit\V2\Exceptions\InvalidArgumentException;
use Zynga\PHPUnit\V2\TestCase;
use Zynga\PHPUnit\V2\TestCase\Status;
use Zynga\PHPUnit\V2\TestSuite;
use Zynga\PHPUnit\V2\Tests\Mock\ChangeCurrentWorkingDirectory;
use Zynga\PHPUnit\V2\Tests\Mock\ExceptionInAssertPreConditions;
use Zynga\PHPUnit\V2\Tests\Mock\ExceptionInAssertPostConditions;
use Zynga\PHPUnit\V2\Tests\Mock\ExceptionInButExpected;
use Zynga\PHPUnit\V2\Tests\Mock\ExceptionInSetup;
use Zynga\PHPUnit\V2\Tests\Mock\ExceptionInTearDown;
use Zynga\PHPUnit\V2\Tests\Mock\ExceptionIn;
use Zynga\PHPUnit\V2\Tests\Mock\Failure;
use Zynga\PHPUnit\V2\Tests\Mock\Isolation;
use Zynga\PHPUnit\V2\Tests\Mock\Mockable;
use Zynga\PHPUnit\V2\Tests\Mock\NoArgTestCase;
use Zynga\PHPUnit\V2\Tests\Mock\OutputTestCase;
use Zynga\PHPUnit\V2\Tests\Mock\Requirements;
use Zynga\PHPUnit\V2\Tests\Mock\Singleton;
use Zynga\PHPUnit\V2\Tests\Mock\Success;
use Zynga\PHPUnit\V2\Tests\Mock\TestError;
use Zynga\PHPUnit\V2\Tests\Mock\TestIncomplete;
use Zynga\PHPUnit\V2\Tests\Mock\TestSkipped;
use Zynga\PHPUnit\V2\Tests\Mock\ThrowExceptionTestCase;
use Zynga\PHPUnit\V2\Tests\Mock\ThrowNoExceptionTestCase;
use Zynga\PHPUnit\V2\Tests\Mock\WasRun;
use Zynga\PHPUnit\V2\TestFailure;
use Zynga\PHPUnit\V2\TestResult;
use Zynga\PHPUnit\V2\Version;
use Zynga\Framework\ReflectionCache\V1\ReflectionClasses;
use Zynga\PHPUnit\V2\Assertions\AssertExceptionRegexpMessage;

use \ReflectionClass;
use \Exception;
use \Throwable;
use \RuntimeException;

// $GLOBALS['a']  = 'a';
// $_ENV['b']     = 'b';
// $_POST['c']    = 'c';
// $_GET['d']     = 'd';
// $_COOKIE['e']  = 'e';
// $_SERVER['f']  = 'f';
// $_FILES['g']   = 'g';
// $_REQUEST['h'] = 'h';
// $GLOBALS['i']  = 'i';

/**
 * @since      Class available since Release 2.0.0
 * @covers     TestCase
 */
class TestCaseTest extends BaseTest {

  public function doSetUpBeforeClass(): void {
    // noop
  }

  public function doTearDownAfterClass(): void {
    // noop
  }

  public function testCaseToString(): void {
    $this->assertEquals(
      'Zynga\PHPUnit\V2\Tests\Framework\TestCaseTest::testCaseToString',
      $this->toString(),
    );
  }

  // private function _debugException(Exception $e): void {

  //   print 'message='."\n";
  //   var_dump($e->getMessage());
  //   print 'file='.$e->getFile().'('.$e->getLine().')'."\n";
  //   print 'trace='."\n";
  //   $offset = 0;
  //   foreach ($e->getTrace() as $traceFrame) {
  //     $file = '';
  //     $class = '';
  //     $function = '';
  //     $line = '';

  //     if (array_key_exists('file', $traceFrame)) {
  //       $file = $traceFrame['file'];
  //     }
  //     if (array_key_exists('class', $traceFrame)) {
  //       $class = $traceFrame['class'].'::';
  //     }
  //     if (array_key_exists('function', $traceFrame)) {
  //       $function = $traceFrame['function'];
  //     }

  //     if (array_key_exists('line', $traceFrame)) {
  //       $line = $traceFrame['line'];
  //     }

  //     print 'frame='.$offset."\n";
  //     print '  file='.$file.'('.$line.') '.$class.$function."\n";

  //     $offset++;
  //   }
  // }

  // private function _debugExceptions(Vector<TestFailure> $exceptions): void {
  //   foreach ($exceptions as $exception) {
  //     $this->_debugException($exception->thrownException());
  //   }
  // }

  // private function _debugTestResult(
  //   TestResult $result,
  //   bool $debug,
  //   int $errorCount = 0,
  //   int $failureCount = 0,
  //   int $skippedCount = 0,
  //   int $incompleteCount = 0,
  // ): void {

  //   if ($debug != true) {
  //     return;
  //   }

  //   $actualErrorCount = $result->errorCount();
  //   $actualErrors = $result->errors();

  //   print
  //     'errorCount expected='.$errorCount.' actual='.$actualErrorCount."\n"
  //   ;
  //   $this->_debugExceptions($actualErrors);

  //   $actualFailureCount = $result->failureCount();
  //   $actualFailures = $result->failures();

  //   print
  //     'failureCount'.
  //     ' expected='.
  //     $failureCount.
  //     ' actual='.
  //     $actualFailureCount.
  //     "\n"
  //   ;

  //   $this->_debugExceptions($actualFailures);

  //   $actualSkippedCount = $result->skippedCount();
  //   $actualSkipped = $result->skipped();

  //   var_dump(
  //     'skippedCount'.
  //     ' expected='.
  //     $skippedCount.
  //     ' actual='.
  //     $actualSkippedCount,
  //   );

  //   $this->_debugExceptions($actualSkipped);

  //   $actualIncompleteCount = $result->notImplementedCount();
  //   $actualIncomplete = $result->notImplemented();

  //   print
  //     'incompleteCount'.
  //     'expected='.
  //     $incompleteCount.
  //     ' actual='.
  //     $actualIncompleteCount.
  //     "\n"
  //   ;

  //   $this->_debugExceptions($actualIncomplete);

  // }

  // private function _verifyTestResult(
  //   TestResult $result,
  //   bool $debug = false,
  //   int $errorCount = 0,
  //   int $failureCount = 0,
  //   int $skippedCount = 0,
  //   int $incompleteCount = 0,
  // ): void {

  //   $this->assertEquals($errorCount, $result->errorCount(), 'errorCount');

  //   $this->assertEquals(
  //     $failureCount,
  //     $result->failureCount(),
  //     'failureCount',
  //   );

  //   $this->assertEquals(
  //     $skippedCount,
  //     $result->skippedCount(),
  //     'skippedCount',
  //   );

  //   $this->assertEquals(
  //     $incompleteCount,
  //     $result->notImplementedCount(),
  //     'incompleteCount',
  //   );

  // }

  // private function _verifyTestSuite(
  //   TestSuite $test,
  //   TestResult $result,
  //   bool $debug = false,
  //   int $errorCount = 0,
  //   int $failureCount = 0,
  //   int $skippedCount = 0,
  //   int $incompleteCount = 0,
  // ): void {

  //   $this->_debugTestResult(
  //     $result,
  //     $debug,
  //     $errorCount,
  //     $failureCount,
  //     $skippedCount,
  //     $incompleteCount,
  //   );

  //   $this->_verifyTestResult(
  //     $result,
  //     $debug,
  //     $errorCount,
  //     $failureCount,
  //     $skippedCount,
  //     $incompleteCount,
  //   );

  // }

  // private function _verifyTest(
  //   TestCase $test,
  //   TestResult $result,
  //   bool $debug = false,
  //   int $statusCode = Status::STATUS_PASSED,
  //   string $statusMessage = '',
  //   int $errorCount = 0,
  //   int $failureCount = 0,
  //   int $skippedCount = 0,
  //   int $incompleteCount = 0,
  // ): void {

  //   if ($debug == true) {

  //     // --
  //     // JEO: Need a better way to dump things into phphunit's output buffer.
  //     // --
  //     var_dump('_verifyTestResults::debug');

  //     var_dump(
  //       'statusCode expected='.$statusCode.' actual='.$test->getStatus(),
  //     );

  //     var_dump('statusMessage expected=');
  //     var_dump($statusMessage);

  //     var_dump('statusMessage actual=');
  //     var_dump($test->getStatusMessage());

  //     print 'expectedOutput='.$test->getExpectedOutput()."\n";
  //     print 'expectedOutputRegex='.$test->getExpectedOutputRegex()."\n";

  //     print 'expectedException='.$test->getExpectedException()."\n";

  //     print
  //       'expectedExceptionMessage='.
  //       $test->getExpectedExceptionMessage().
  //       "\n"
  //     ;

  //     print
  //       'expectedExceptionMessageRegExp='.
  //       $test->getExpectedExceptionMessageRegExp().
  //       "\n"
  //     ;

  //     $this->_debugTestResult(
  //       $result,
  //       $debug,
  //       $errorCount,
  //       $failureCount,
  //       $skippedCount,
  //       $incompleteCount,
  //     );

  //   }

  //   $this->assertEquals($statusCode, $test->getStatus());
  //   $this->assertEquals($statusMessage, $test->getStatusMessage());

  //   $this->_verifyTestResult(
  //     $result,
  //     $debug,
  //     $errorCount,
  //     $failureCount,
  //     $skippedCount,
  //     $incompleteCount,
  //   );

  // }

  public function testSuccess(): void {

    $test = new Success('testNoop');

    $result = $test->run();

    $this->assertEquals(1, count($result));

    $this->_verifyTest(
      $test,
      $result,
      false,
      Status::STATUS_PASSED,
      '',
      0,
      0,
      0,
    );

  }

  public function testFailure(): void {
    $test = new Failure('testFailure');
    $result = $test->run();

    $this->assertEquals(1, count($result));

    $this->_verifyTest(
      $test,
      $result,
      false,
      Status::STATUS_FAILURE,
      '',
      0,
      1,
      0,
    );

  }

  public function testError(): void {

    $test = new TestError('testError');

    $result = $test->run();

    $this->assertEquals(1, count($result));

    $this->_verifyTest(
      $test,
      $result,
      false,
      Status::STATUS_ERROR,
      'Mock-Test-Error',
      1,
      0,
      0,
    );

  }

  public function testSkipped(): void {
    $test = new TestSkipped('testSkip');
    $result = $test->run();

    $this->assertEquals(1, count($result));

    $this->_verifyTest(
      $test,
      $result,
      false,
      Status::STATUS_SKIPPED,
      'Skipped test',
      0,
      0,
      1,
    );

  }

  public function testIncomplete(): void {

    $test = new TestIncomplete('testMarkIncomplete');

    $result = $test->run();

    $this->assertEquals(1, count($result));

    $this->_verifyTest(
      $test,
      $result,
      false,
      Status::STATUS_INCOMPLETE,
      'Incomplete test',
      0,
      0,
      0,
      1,
    );

  }

  public function testExceptionInSetUp(): void {

    $test = new ExceptionInSetup('testSomething');
    $result = $test->run();

    $this->assertTrue($test->setUp);
    $this->assertFalse($test->assertPreConditions);
    $this->assertFalse($test->testSomething);
    $this->assertFalse($test->assertPostConditions);
    $this->assertTrue($test->tearDown);

  }

  public function testExceptionInAssertPreConditions(): void {

    $test = new ExceptionInAssertPreConditions('testSomething');
    $result = $test->run();

    $this->assertTrue($test->setUp);
    $this->assertTrue($test->assertPreConditions);
    $this->assertFalse($test->testSomething);
    $this->assertFalse($test->assertPostConditions);
    $this->assertTrue($test->tearDown);
  }

  public function testExceptionInTest(): void {

    $test = new ExceptionIn('testSomething');
    $result = $test->run();

    $this->assertTrue($test->setUp);
    $this->assertTrue($test->assertPreConditions);
    $this->assertTrue($test->testSomething);
    $this->assertFalse($test->assertPostConditions);
    $this->assertTrue($test->tearDown);

  }

  public function testExceptionInAssertPostConditions(): void {

    $test = new ExceptionInAssertPostConditions('testSomething');
    $result = $test->run();

    $this->assertTrue($test->setUp);
    $this->assertTrue($test->assertPreConditions);
    $this->assertTrue($test->testSomething);
    $this->assertTrue($test->assertPostConditions);
    $this->assertTrue($test->tearDown);
  }

  public function testExceptionInTearDown(): void {
    $test = new ExceptionInTearDown('testSomething');
    $result = $test->run();

    $this->assertTrue($test->setUp);
    $this->assertTrue($test->assertPreConditions);
    $this->assertTrue($test->testSomething);
    $this->assertTrue($test->assertPostConditions);
    $this->assertTrue($test->tearDown);
  }

  public function testNoArgTestCasePasses(): void {

    $suite = new TestSuite(NoArgTestCase::class);

    $result = $suite->run();

    $this->_verifyTestSuite($suite, $result, false, 1, 0, 0, 0);

  }

  public function testWasRun(): void {

    $test = new WasRun('testWasRun');

    $result = $test->run();

    $this->assertEquals(1, count($result));

    $this->_verifyTest(
      $test,
      $result,
      false,
      Status::STATUS_PASSED,
      '',
      0,
      0,
      0,
    );

    $this->assertTrue($test->wasRun);

  }

  public function testException(): void {
    $test = new ThrowExceptionTestCase('test');
    $test->expectException(RuntimeException::class);

    $result = $test->run();

    $this->assertEquals(1, count($result));
    $this->assertTrue($result->wasSuccessful());
  }

  public function testExceptionWithEmptyMessage(): void {
    $test = new ThrowExceptionTestCase('test');
    $test->expectException(RuntimeException::class);

    $result = $test->run();

    $this->assertEquals(1, count($result));
    $this->assertTrue($result->wasSuccessful());
  }

  public function testExceptionWithNullMessage(): void {
    $test = new ThrowExceptionTestCase('test');
    $test->expectException(RuntimeException::class);

    $result = $test->run();

    $this->assertEquals(1, count($result));

    $this->_verifyTest(
      $test,
      $result,
      false,
      Status::STATUS_PASSED,
      '',
      0,
      0,
      0,
      0,
    );

    //$this->assertTrue($result->wasSuccessful());
  }

  public function testExceptionWithMessage(): void {
    $test = new ThrowExceptionTestCase('test');
    $test->expectException(RuntimeException::class);
    $test->expectExceptionMessage('MOCK: A runtime error occurred');

    $result = $test->run();

    $this->_verifyTest(
      $test,
      $result,
      false,
      Status::STATUS_PASSED,
      '',
      0,
      0,
      0,
      0,
    );

  }

  public function testExceptionWithWrongMessage(): void {

    $test = new ThrowExceptionTestCase('test');
    $test->expectException(RuntimeException::class);
    // JEO: Purposely set the message to a non-matching value, in this case 'logic' isn't in the exception message.
    $test->expectExceptionMessage('MOCK: A logic error occurred');

    $result = $test->run();

    $this->assertEquals(1, count($result));

    $this->_verifyTest(
      $test,
      $result,
      false,
      Status::STATUS_FAILURE,
      'Failed asserting that \'MOCK: A runtime error occurred\' is equal to <string:MOCK: A logic error occurred>.',
      0,
      1,
      0,
    );

  }

  public function testExceptionWithRegexpMessage(): void {
    $test = new ThrowExceptionTestCase('test');
    $test->expectException(RuntimeException::class);
    $test->expectExceptionMessageRegExp('/runtime .*? occurred/');

    $result = $test->run();

    $this->assertEquals(1, count($result));

    $this->assertTrue($result->wasSuccessful());
  }

  public function testExceptionWithWrongRegexpMessage(): void {
    $test = new ThrowExceptionTestCase('test');
    $test->expectException(RuntimeException::class);

    // JEO: Purposely set the message to a non-matching value, in this case 'logic' isn't in the exception message.
    $test->expectExceptionMessageRegExp('/logic .*? occurred/');

    $result = $test->run();

    $this->assertEquals(1, count($result));

    $this->_verifyTest(
      $test,
      $result,
      false,
      Status::STATUS_FAILURE,
      "Failed asserting that 'MOCK: A runtime error occurred' matches PCRE pattern \"/logic .*? occurred/\".",
      0,
      1,
      0,
      0,
    );

  }

  public function testExceptionWithInvalidRegexpMessage(): void {
    $test = new ThrowExceptionTestCase('test');
    $test->expectException(RuntimeException::class);

    // JEO: Purposely set a broken regexp to see the handling for that situation.
    $test->expectExceptionMessageRegExp('#runtime .*? occurred/'); // wrong delimiter

    $result = $test->run();

    $reflect =
      ReflectionClasses::getReflection(AssertExceptionRegexpMessage::class);

    $filePath = 'unable-to-resolve';

    if ($reflect instanceof ReflectionClass) {
      $filePath = strval($reflect->getFileName());
    }

    $this->_verifyTest(
      $test,
      $result,
      false,
      Status::STATUS_FAILURE,
      "Invalid regex provided: No ending delimiter '#' found: [#runtime .*? occurred/] file=$filePath:30",
      0,
      1,
      0,
      0,
    );

  }

  public function testNoException(): void {

    $test = new ThrowNoExceptionTestCase('test');
    $test->expectException(RuntimeException::class);

    $result = $test->run();

    $this->_verifyTest(
      $test,
      $result,
      false,
      Status::STATUS_FAILURE,
      'Expected=RuntimeException was presented e=null',
      0,
      1,
      0,
      0,
    );

  }

  public function testWrongException(): void {

    $test = new ThrowExceptionTestCase('test');

    $test->expectException(InvalidArgumentException::class);

    $result = $test->run();

    // Failed asserting that RuntimeException Object (...) is an instance of class "Zynga\PHPUnit\V2\Exceptions\InvalidArgumentException".'
    // Failed asserting that 'RuntimeException' Object (...) is equal to <string:Zynga\PHPUnit\V2\Exceptions\InvalidArgumentException>.'
    // Failed asserting that RuntimeException Object (...) is an instance of class "Zynga\PHPUnit\V2\Exceptions\InvalidArgumentException".

    $this->assertEquals(1, count($result));

    $this->_verifyTest(
      $test,
      $result,
      false,
      Status::STATUS_FAILURE,
      "Failed asserting that RuntimeException Object (...) is an instance of class \"Zynga\PHPUnit\V2\Exceptions\InvalidArgumentException\".",
      0,
      1,
      0,
      0,
    );

    //$this->assertEquals(1, $result->failureCount());
    //$this->assertEquals(1, count($result));
  }

  /**
   * @backupGlobals enabled
   */
  // public function testGlobalsBackupPre()
  // {
  //     global $a;
  //     global $i;
  //
  //     $this->assertEquals('a', $a);
  //     $this->assertEquals('a', $GLOBALS['a']);
  //     $this->assertEquals('b', $_ENV['b']);
  //     $this->assertEquals('c', $_POST['c']);
  //     $this->assertEquals('d', $_GET['d']);
  //     $this->assertEquals('e', $_COOKIE['e']);
  //     $this->assertEquals('f', $_SERVER['f']);
  //     $this->assertEquals('g', $_FILES['g']);
  //     $this->assertEquals('h', $_REQUEST['h']);
  //     $this->assertEquals('i', $i);
  //     $this->assertEquals('i', $GLOBALS['i']);
  //
  //     $GLOBALS['a']   = 'aa';
  //     $GLOBALS['foo'] = 'bar';
  //     $_ENV['b']      = 'bb';
  //     $_POST['c']     = 'cc';
  //     $_GET['d']      = 'dd';
  //     $_COOKIE['e']   = 'ee';
  //     $_SERVER['f']   = 'ff';
  //     $_FILES['g']    = 'gg';
  //     $_REQUEST['h']  = 'hh';
  //     $GLOBALS['i']   = 'ii';
  //
  //     $this->assertEquals('aa', $a);
  //     $this->assertEquals('aa', $GLOBALS['a']);
  //     $this->assertEquals('bar', $GLOBALS['foo']);
  //     $this->assertEquals('bb', $_ENV['b']);
  //     $this->assertEquals('cc', $_POST['c']);
  //     $this->assertEquals('dd', $_GET['d']);
  //     $this->assertEquals('ee', $_COOKIE['e']);
  //     $this->assertEquals('ff', $_SERVER['f']);
  //     $this->assertEquals('gg', $_FILES['g']);
  //     $this->assertEquals('hh', $_REQUEST['h']);
  //     $this->assertEquals('ii', $i);
  //     $this->assertEquals('ii', $GLOBALS['i']);
  // }

  // public function testGlobalsBackupPost()
  // {
  //     global $a;
  //     global $i;
  //
  //     $this->assertEquals('a', $a);
  //     $this->assertEquals('a', $GLOBALS['a']);
  //     $this->assertEquals('b', $_ENV['b']);
  //     $this->assertEquals('c', $_POST['c']);
  //     $this->assertEquals('d', $_GET['d']);
  //     $this->assertEquals('e', $_COOKIE['e']);
  //     $this->assertEquals('f', $_SERVER['f']);
  //     $this->assertEquals('g', $_FILES['g']);
  //     $this->assertEquals('h', $_REQUEST['h']);
  //     $this->assertEquals('ii', $i);
  //     $this->assertEquals('ii', $GLOBALS['i']);
  //
  //     $this->assertArrayNotHasKey('foo', $GLOBALS);
  // }

  // JEO: We removed the backupGlobals attributes, therefor this test is invalid.
  // /**
  //  * @backupGlobals enabled
  //  * @backupStaticAttributes enabled
  //  */
  // public function testStaticAttributesBackupPre() {
  //   $GLOBALS['singleton'] = Singleton::getInstance();
  //   self::$_testStatic = 123;
  // }

  /**
   * depends testStaticAttributesBackupPre
   */
  // public function testStaticAttributesBackupPost() {
  //   $this->assertNotSame($GLOBALS['singleton'], Singleton::getInstance());
  //   $this->assertSame(0, self::$_testStatic);
  // }

  // JEO: We removed isolation mode
  // public function testIsInIsolationReturnsFalse() {
  //   $test = new Isolation('testIsInIsolationReturnsFalse');
  //   $result = $test->run();
  //
  //   $this->assertEquals(1, count($result));
  //   $this->assertTrue($result->wasSuccessful());
  // }

  // JEO: We removed that isolation mode
  // public function testIsInIsolationReturnsTrue() {
  //   $test = new Isolation('testIsInIsolationReturnsTrue');
  //   $test->setRunTestInSeparateProcess(true);
  //   $result = $test->run();
  //
  //   $this->assertEquals(1, count($result));
  //   $this->assertTrue($result->wasSuccessful());
  // }

  public function testExpectOutputStringFooActualFoo(): void {

    $test = new OutputTestCase('testExpectOutputStringFooActualFoo');

    $result = $test->run();

    $this->assertEquals(1, count($result));

    $this->_verifyTest(
      $test,
      $result,
      false,
      Status::STATUS_PASSED,
      '',
      0,
      0,
      0,
      0,
    );

  }

  public function testExpectOutputStringFooActualBar(): void {
    $test = new OutputTestCase('testExpectOutputStringFooActualBar');
    $result = $test->run();

    $this->assertEquals(1, count($result));

    $this->_verifyTest(
      $test,
      $result,
      false,
      Status::STATUS_FAILURE,
      'Failed asserting that \'bar\' is equal to <string:foo>.',
      0,
      1,
      0,
      0,
    );

  }

  public function testExpectOutputRegexFooActualFoo(): void {
    $test = new OutputTestCase('testExpectOutputRegexFooActualFoo');
    $result = $test->run();

    $this->assertEquals(1, count($result));
    $this->assertTrue($result->wasSuccessful());
  }

  public function testExpectOutputRegexFooActualBar(): void {
    $test = new OutputTestCase('testExpectOutputRegexFooActualBar');
    $result = $test->run();

    $this->assertEquals(1, count($result));

    $this->_verifyTest(
      $test,
      $result,
      false,
      Status::STATUS_FAILURE,
      'Failed asserting that \'bar\' matches PCRE pattern "/foo/".',
      0,
      1,
      0,
      0,
    );

    $this->assertFalse($result->wasSuccessful());
  }

  public function testSkipsIfRequiresHigherVersionOfPHPUnit(): void {
    $test = new Requirements('testAlwaysSkip');
    $result = $test->run();

    $this->_verifyTest(
      $test,
      $result,
      false,
      Status::STATUS_SKIPPED,
      'PHPUnit version '.Version::VERSION_NUMBER.' >= 1111111 is required.',
      0,
      0,
      1,
      0,
    );

  }

  public function testSkipsIfRequiresHigherVersionOfPHP(): void {

    $test = new Requirements('testAlwaysSkip2');

    $result = $test->run();

    $this->_verifyTest(
      $test,
      $result,
      false,
      Status::STATUS_SKIPPED,
      'PHP version '.PHP_VERSION.' >= 9999999 is required.',
      0,
      0,
      1,
      0,
    );

  }

  public function testSkipsIfRequiresNonExistingOs(): void {
    $test = new Requirements('testAlwaysSkip3');
    $result = $test->run();

    $this->_verifyTest(
      $test,
      $result,
      false,
      Status::STATUS_SKIPPED,
      'Operating system '.PHP_OS.' expected /DOESNOTEXIST/i is required.',
      0,
      0,
      1,
      0,
    );

  }

  public function testSkipsIfRequiresNonExistingFunction(): void {
    $test = new Requirements('testNine');
    $result = $test->run();

    $this->_verifyTest(
      $test,
      $result,
      false,
      Status::STATUS_SKIPPED,
      'Function testFuncThatDoesNotExist is required.',
      0,
      0,
      1,
      0,
    );

  }

  public function testSkipsIfRequiresNonExistingExtension(): void {
    $test = new Requirements('testTen');
    $result = $test->run();

    $this->_verifyTest(
      $test,
      $result,
      false,
      Status::STATUS_SKIPPED,
      'Extension testExt is required.',
      0,
      0,
      1,
      0,
    );

  }

  public function testSkipsIfRequiresExtensionWithAMinimumVersion(): void {

    $test = new Requirements('testSpecificExtensionVersion');

    $result = $test->run();

    $this->_verifyTest(
      $test,
      $result,
      false,
      Status::STATUS_SKIPPED,
      'Extension testExt is required to be >= 1.8.0.',
      0,
      0,
      1,
      0,
    );

  }

  public function testSkipsProvidesMessagesForAllSkippingReasons(): void {
    $test = new Requirements('testAllPossibleRequirements');
    $result = $test->run();

    $expectedOutput = '';
    $expectedOutput .=
      'PHP version '.PHP_VERSION.' >= 99-dev is required.'.PHP_EOL;
    $expectedOutput .=
      'PHPUnit version '.
      Version::VERSION_NUMBER.
      ' >= 9-dev is required.'.
      PHP_EOL;
    $expectedOutput .=
      'Operating system '.
      PHP_OS.
      ' expected /DOESNOTEXIST/i is required.'.
      PHP_EOL;
    $expectedOutput .= 'Function testFuncOne is required.'.PHP_EOL;
    $expectedOutput .= 'Function testFuncTwo is required.'.PHP_EOL;
    $expectedOutput .= 'Extension testExtOne is required.'.PHP_EOL;
    $expectedOutput .= 'Extension testExtTwo is required.'.PHP_EOL;
    $expectedOutput .= 'Extension testExtThree is required to be >= 2.0.';

    $this->_verifyTest(
      $test,
      $result,
      true,
      Status::STATUS_SKIPPED,
      $expectedOutput,
      0,
      0,
      1,
      0,
    );

  }

  public function testRequiringAnExistingMethodDoesNotSkip(): void {
    $test = new Requirements('testExistingMethod');
    $result = $test->run();
    $this->assertEquals(0, $result->skippedCount());
  }

  public function testRequiringAnExistingFunctionDoesNotSkip(): void {
    $test = new Requirements('testExistingFunction');
    $result = $test->run();
    $this->assertEquals(0, $result->skippedCount());
  }

  public function testRequiringAnExistingExtensionDoesNotSkip(): void {
    $test = new Requirements('testExistingExtension');
    $result = $test->run();
    $this->assertEquals(0, $result->skippedCount());
  }

  public function testRequiringAnExistingOsDoesNotSkip(): void {
    $test = new Requirements('testExistingOs');
    $result = $test->run();
    $this->assertEquals(0, $result->skippedCount());
  }

  public function testCurrentWorkingDirectoryIsRestored(): void {
    $expectedCwd = getcwd();

    $test =
      new ChangeCurrentWorkingDirectory('testSomethingThatChangesTheCwd');
    $test->run();

    $this->assertSame($expectedCwd, getcwd());
  }

  public function testExpectedExceptionInComments(): void {

    $test = new ExceptionInButExpected('testSomething');

    $result = $test->run();

    $this->_verifyTest(
      $test,
      $result,
      false,
      Status::STATUS_PASSED,
      '',
      0,
      0,
      0,
    );

  }

  // --
  // JEO: createMock is deprecated
  // --
  // public function testCreateMockFromClassName() {
  //   $mock = $this->createMock(Mockable::class);
  //
  //   $this->assertInstanceOf(Mockable::class, $mock);
  //   $this->assertInstanceOf(
  //     PHPUnit_Framework_MockObject_MockObject::class,
  //     $mock,
  //   );
  // }

  // --
  // JEO: createMock is deprecated
  // --
  // public function testCreateMockMocksAllMethods() {
  //   /** @var Mockable $mock */
  //   $mock = $this->createMock(Mockable::class);
  //
  //   $this->assertNull($mock->foo());
  // }

  // --
  // JEO: createMock is deprecated
  // --
  // public function testCreateMockSkipsConstructor() {
  //   $mock = $this->createMock(Mockable::class);
  //
  //   $this->assertFalse($mock->constructorCalled);
  // }

  // --
  // JEO: createMock is deprecated
  // --
  // public function testCreateMockDisablesOriginalClone() {
  //   $mock = $this->createMock(Mockable::class);
  //
  //   $cloned = clone $mock;
  //   $this->assertFalse($cloned->cloned);
  // }

  /**
   * @depends testExpectedExceptionInComments
   */
  public function testDependsTag(): void {
    $this->assertTrue(true);
  }

}
