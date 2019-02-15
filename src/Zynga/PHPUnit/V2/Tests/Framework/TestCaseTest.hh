<?hh // partial

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
use Zynga\PHPUnit\V2\Tests\Framework\BaseTest;
use Zynga\PHPUnit\V2\Tests\Mock\ChangeCurrentWorkingDirectory;
use Zynga\PHPUnit\V2\Tests\Mock\ExceptionInAssertPreConditions;
use Zynga\PHPUnit\V2\Tests\Mock\ExceptionInAssertPostConditions;
use Zynga\PHPUnit\V2\Tests\Mock\ExceptionInSetUp;
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

use \PHPUnit_Framework_TestCase;
use \PHPUnit_Framework_TestResult;
use \PHPUnit_Framework_TestSuite;
use \PHPUnit_Runner_BaseTestRunner;

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
 * @covers     PHPUnit_Framework_TestCase
 */
class TestCaseTest extends PHPUnit_Framework_TestCase {

  public function doSetUpBeforeClass(): void {
    // noop
  }

  public function doTearDownAfterClass(): void {
    // noop
  }

  protected $backupGlobalsBlacklist = ['i', 'singleton'];

  /**
   * Used be testStaticAttributesBackupPre
   */
  protected static $_testStatic = 0;

  public function testCaseToString() {
    $this->assertEquals(
      'Zynga\PHPUnit\V2\Tests\Framework\TestCaseTest::testCaseToString',
      $this->toString(),
    );
  }

  private function _debugTestResult(
    PHPUnit_Framework_TestResult $result,
    bool $debug,
    int $errorCount = 0,
    int $failureCount = 0,
    int $skippedCount = 0,
  ): void {
    if ($debug != true) {
      return;
    }

    $actualErrorCount = $result->errorCount();
    $actualErrors = $result->errors();

    var_dump('errorCount expected='.$errorCount.' actual='.$actualErrorCount);
    var_dump($actualErrors);

    $actualFailureCount = $result->failureCount();
    $actualFailures = $result->failures();

    var_dump(
      'failureCount'.
      ' expected='.
      $failureCount.
      ' actual='.
      $actualFailureCount,
    );
    var_dump($actualFailures);

    $actualSkippedCount = $result->skippedCount();
    $actualSkipped = $result->skipped();

    var_dump(
      'skippedCount'.
      ' expected='.
      $skippedCount.
      ' actual='.
      $actualSkippedCount,
    );
    var_dump($actualSkipped);

  }

  private function _verifyTestResult(
    PHPUnit_Framework_TestResult $result,
    bool $debug = false,
    int $errorCount = 0,
    int $failureCount = 0,
    int $skippedCount = 0,
  ): void {

    $this->assertEquals($errorCount, $result->errorCount());
    $this->assertEquals($failureCount, $result->failureCount());
    $this->assertEquals($skippedCount, $result->skippedCount());

  }

  private function _verifyTestSuite(
    PHPUnit_Framework_TestSuite $test,
    PHPUnit_Framework_TestResult $result,
    bool $debug = false,
    int $errorCount = 0,
    int $failureCount = 0,
    int $skippedCount = 0,
  ): void {

    $this->_debugTestResult(
      $result,
      $debug,
      $errorCount,
      $failureCount,
      $skippedCount,
    );

    $this->_verifyTestResult(
      $result,
      $debug,
      $errorCount,
      $failureCount,
      $skippedCount,
    );

  }

  private function _verifyTest(
    PHPUnit_Framework_TestCase $test,
    PHPUnit_Framework_TestResult $result,
    bool $debug = false,
    int $statusCode = PHPUnit_Runner_BaseTestRunner::STATUS_PASSED,
    string $statusMessage = '',
    int $errorCount = 0,
    int $failureCount = 0,
    int $skippedCount = 0,
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

      $this->_debugTestResult(
        $result,
        $debug,
        $errorCount,
        $failureCount,
        $skippedCount,
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
    );

  }

  public function testSuccess() {

    $test = new Success();

    $result = $test->run();

    $this->assertEquals(1, count($result));

    $this->_verifyTest(
      $test,
      $result,
      false,
      PHPUnit_Runner_BaseTestRunner::STATUS_PASSED,
      '',
      0,
      0,
      0,
    );

  }

  public function testFailure() {
    $test = new Failure();
    $result = $test->run();

    $this->assertEquals(1, count($result));

    $this->_verifyTest(
      $test,
      $result,
      false,
      PHPUnit_Runner_BaseTestRunner::STATUS_FAILURE,
      '',
      0,
      1,
      0,
    );

  }

  public function testError() {

    $test = new TestError();

    $result = $test->run();

    $this->assertEquals(1, count($result));

    $this->_verifyTest(
      $test,
      $result,
      false,
      PHPUnit_Runner_BaseTestRunner::STATUS_ERROR,
      '',
      1,
      0,
      0,
    );

  }

  public function testSkipped() {
    $test = new TestSkipped();
    $result = $test->run();

    $this->assertEquals(1, count($result));

    $this->_verifyTest(
      $test,
      $result,
      false,
      PHPUnit_Runner_BaseTestRunner::STATUS_SKIPPED,
      'Skipped test',
      0,
      0,
      1,
    );

  }

  public function testIncomplete() {

    $test = new TestIncomplete();

    $result = $test->run();

    $this->assertEquals(1, count($result));

    $this->_verifyTest(
      $test,
      $result,
      false,
      PHPUnit_Runner_BaseTestRunner::STATUS_INCOMPLETE,
      'Incomplete test',
      1,
      0,
      0,
    );

  }

  public function testExceptionInSetUp() {
    $test = new ExceptionInSetUp('testSomething');
    $result = $test->run();

    $this->assertTrue($test->setUp);
    $this->assertFalse($test->assertPreConditions);
    $this->assertFalse($test->testSomething);
    $this->assertFalse($test->assertPostConditions);
    $this->assertTrue($test->tearDown);
  }

  public function testExceptionInAssertPreConditions() {
    $test = new ExceptionInAssertPreConditions('testSomething');
    $result = $test->run();

    $this->assertTrue($test->setUp);
    $this->assertTrue($test->assertPreConditions);
    $this->assertFalse($test->testSomething);
    $this->assertFalse($test->assertPostConditions);
    $this->assertTrue($test->tearDown);
  }

  public function testExceptionInTest() {
    $test = new ExceptionIn('testSomething');
    $result = $test->run();

    $this->assertTrue($test->setUp);
    $this->assertTrue($test->assertPreConditions);
    $this->assertTrue($test->testSomething);
    $this->assertFalse($test->assertPostConditions);
    $this->assertTrue($test->tearDown);
  }

  public function testExceptionInAssertPostConditions() {
    $test = new ExceptionInAssertPostConditions('testSomething');
    $result = $test->run();

    $this->assertTrue($test->setUp);
    $this->assertTrue($test->assertPreConditions);
    $this->assertTrue($test->testSomething);
    $this->assertTrue($test->assertPostConditions);
    $this->assertTrue($test->tearDown);
  }

  public function testExceptionInTearDown() {
    $test = new ExceptionInTearDown('testSomething');
    $result = $test->run();

    $this->assertTrue($test->setUp);
    $this->assertTrue($test->assertPreConditions);
    $this->assertTrue($test->testSomething);
    $this->assertTrue($test->assertPostConditions);
    $this->assertTrue($test->tearDown);
  }

  public function testNoArgTestCasePasses() {

    $result = new PHPUnit_Framework_TestResult();

    $suite = new PHPUnit_Framework_TestSuite(NoArgTestCase::class);

    $suite->run($result);

    $this->assertEquals(1, count($result));

    $this->_verifyTestSuite($suite, $result, false, 0, 0, 0);

  }

  public function testWasRun() {

    $test = new WasRun();

    $result = $test->run();

    $this->assertEquals(1, count($result));

    $this->_verifyTest(
      $test,
      $result,
      false,
      PHPUnit_Runner_BaseTestRunner::STATUS_PASSED,
      '',
      0,
      0,
      0,
    );

    $this->assertTrue($test->wasRun);

  }

  public function testException() {
    $test = new ThrowExceptionTestCase('test');
    $test->expectException(RuntimeException::class);

    $result = $test->run();

    $this->assertEquals(1, count($result));
    $this->assertTrue($result->wasSuccessful());
  }

  public function testExceptionWithEmptyMessage() {
    $test = new ThrowExceptionTestCase('test');
    $test->expectException(RuntimeException::class);

    $result = $test->run();

    $this->assertEquals(1, count($result));
    $this->assertTrue($result->wasSuccessful());
  }

  public function testExceptionWithNullMessage() {
    $test = new ThrowExceptionTestCase('test');
    $test->expectException(RuntimeException::class);

    $result = $test->run();

    $this->assertEquals(1, count($result));
    $this->assertTrue($result->wasSuccessful());
  }

  public function testExceptionWithMessage() {
    $test = new ThrowExceptionTestCase('test');
    $test->expectException(RuntimeException::class);
    $test->expectExceptionMessage('A runtime error occurred');

    $result = $test->run();

    $this->assertEquals(1, count($result));
    $this->assertTrue($result->wasSuccessful());
  }

  public function testExceptionWithWrongMessage() {

    $test = new ThrowExceptionTestCase('test');
    $test->expectException(RuntimeException::class);
    $test->expectExceptionMessage('A logic error occurred');

    $result = $test->run();

    $this->assertEquals(1, count($result));

    $this->_verifyTest(
      $test,
      $result,
      false,
      PHPUnit_Runner_BaseTestRunner::STATUS_FAILURE,
      'Failed asserting that \'A runtime error occurred\' is equal to <string:A logic error occurred>.',
      0,
      1,
      0,
    );

  }

  public function testExceptionWithRegexpMessage() {
    $test = new ThrowExceptionTestCase('test');
    $test->expectException(RuntimeException::class);
    $test->expectExceptionMessageRegExp('/runtime .*? occurred/');

    $result = $test->run();

    $this->assertEquals(1, count($result));

    $this->assertTrue($result->wasSuccessful());
  }

  public function testExceptionWithWrongRegexpMessage() {
    $test = new ThrowExceptionTestCase('test');
    $test->expectException(RuntimeException::class);
    $test->expectExceptionMessageRegExp('/logic .*? occurred/');

    $result = $test->run();

    $this->assertEquals(1, count($result));

    $this->_verifyTest(
      $test,
      $result,
      false,
      PHPUnit_Runner_BaseTestRunner::STATUS_FAILURE,
      'Failed asserting that \'A runtime error occurred\' matches PCRE pattern "/logic .*? occurred/".',
      0,
      1,
      0,
    );

  }

  public function testExceptionWithInvalidRegexpMessage() {
    $test = new ThrowExceptionTestCase('test');
    $test->expectException(RuntimeException::class);
    $test->expectExceptionMessageRegExp('#runtime .*? occurred/'); // wrong delimiter

    $result = $test->run();

    $this->assertEquals(
      "Invalid regex provided: No ending delimiter '#' found: [#runtime .*? occurred/]",
      $test->getStatusMessage(),
    );

  }

  public function testNoException() {
    $test = new ThrowNoExceptionTestCase('test');
    $test->expectException(RuntimeException::class);

    $result = $test->run();

    $this->assertEquals(1, $result->failureCount());
    $this->assertEquals(1, count($result));
  }

  public function testWrongException() {
    $test = new ThrowExceptionTestCase('test');
    $test->expectException(InvalidArgumentException::class);

    $result = $test->run();

    $this->assertEquals(1, $result->failureCount());
    $this->assertEquals(1, count($result));
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

  public function testExpectOutputStringFooActualFoo() {

    $test = new OutputTestCase('testExpectOutputStringFooActualFoo');

    $result = $test->run();

    $this->assertEquals(1, count($result));

    $this->_verifyTest(
      $test,
      $result,
      false,
      PHPUnit_Runner_BaseTestRunner::STATUS_PASSED,
      '',
      0,
      0,
      0,
    );

  }

  public function testExpectOutputStringFooActualBar() {
    $test = new OutputTestCase('testExpectOutputStringFooActualBar');
    $result = $test->run();

    $this->assertEquals(1, count($result));
    $this->assertFalse($result->wasSuccessful());
  }

  public function testExpectOutputRegexFooActualFoo() {
    $test = new OutputTestCase('testExpectOutputRegexFooActualFoo');
    $result = $test->run();

    $this->assertEquals(1, count($result));
    $this->assertTrue($result->wasSuccessful());
  }

  public function testExpectOutputRegexFooActualBar() {
    $test = new OutputTestCase('testExpectOutputRegexFooActualBar');
    $result = $test->run();

    $this->assertEquals(1, count($result));
    $this->assertFalse($result->wasSuccessful());
  }

  public function testSkipsIfRequiresHigherVersionOfPHPUnit() {
    $test = new Requirements('testAlwaysSkip');
    $result = $test->run();

    $this->assertEquals(1, $result->skippedCount());
    $this->assertEquals(
      'PHPUnit >= 1111111 is required.',
      $test->getStatusMessage(),
    );
  }

  public function testSkipsIfRequiresHigherVersionOfPHP() {
    $test = new Requirements('testAlwaysSkip2');
    $result = $test->run();

    $this->assertEquals(1, $result->skippedCount());
    $this->assertEquals(
      'PHP >= 9999999 is required.',
      $test->getStatusMessage(),
    );
  }

  public function testSkipsIfRequiresNonExistingOs() {
    $test = new Requirements('testAlwaysSkip3');
    $result = $test->run();

    $this->assertEquals(1, $result->skippedCount());
    $this->assertEquals(
      'Operating system matching /DOESNOTEXIST/i is required.',
      $test->getStatusMessage(),
    );
  }

  public function testSkipsIfRequiresNonExistingFunction() {
    $test = new Requirements('testNine');
    $result = $test->run();

    $this->assertEquals(1, $result->skippedCount());
    $this->assertEquals(
      'Function testFuncThatDoesNotExist is required.',
      $test->getStatusMessage(),
    );
  }

  public function testSkipsIfRequiresNonExistingExtension() {
    $test = new Requirements('testTen');
    $result = $test->run();

    $this->assertEquals(
      'Extension testExt is required.',
      $test->getStatusMessage(),
    );
  }

  public function testSkipsIfRequiresExtensionWithAMinimumVersion() {
    $test = new Requirements('testSpecificExtensionVersion');
    $result = $test->run();

    $this->assertEquals(
      'Extension testExt >= 1.8.0 is required.',
      $test->getStatusMessage(),
    );
  }

  public function testSkipsProvidesMessagesForAllSkippingReasons() {
    $test = new Requirements('testAllPossibleRequirements');
    $result = $test->run();

    $this->assertEquals(
      'PHP >= 99-dev is required.'.
      PHP_EOL.
      'PHPUnit >= 9-dev is required.'.
      PHP_EOL.
      'Operating system matching /DOESNOTEXIST/i is required.'.
      PHP_EOL.
      'Function testFuncOne is required.'.
      PHP_EOL.
      'Function testFuncTwo is required.'.
      PHP_EOL.
      'Extension testExtOne is required.'.
      PHP_EOL.
      'Extension testExtTwo is required.'.
      PHP_EOL.
      'Extension testExtThree >= 2.0 is required.',
      $test->getStatusMessage(),
    );
  }

  public function testRequiringAnExistingMethodDoesNotSkip() {
    $test = new Requirements('testExistingMethod');
    $result = $test->run();
    $this->assertEquals(0, $result->skippedCount());
  }

  public function testRequiringAnExistingFunctionDoesNotSkip() {
    $test = new Requirements('testExistingFunction');
    $result = $test->run();
    $this->assertEquals(0, $result->skippedCount());
  }

  public function testRequiringAnExistingExtensionDoesNotSkip() {
    $test = new Requirements('testExistingExtension');
    $result = $test->run();
    $this->assertEquals(0, $result->skippedCount());
  }

  public function testRequiringAnExistingOsDoesNotSkip() {
    $test = new Requirements('testExistingOs');
    $result = $test->run();
    $this->assertEquals(0, $result->skippedCount());
  }

  public function testCurrentWorkingDirectoryIsRestored() {
    $expectedCwd = getcwd();

    $test =
      new ChangeCurrentWorkingDirectory('testSomethingThatChangesTheCwd');
    $test->run();

    $this->assertSame($expectedCwd, getcwd());
  }

  // /**
  //  * @requires PHP 7
  //  * @expectedException TypeError
  //  */
  // public function testTypeErrorCanBeExpected() {
  //   $o = new ClassWithScalarTypeDeclarations();
  //   $o->foo(null, null);
  // }

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

}
