<?hh // strict

namespace Zynga\PHPUnit\V2\Tests\System;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */
use Zynga\PHPUnit\V2\TestCase\Status;
use Zynga\PHPUnit\V2\TestResult;
use Zynga\PHPUnit\V2\TestSuite;
use Zynga\PHPUnit\V2\Tests\System\BaseTest;
use Zynga\PHPUnit\V2\Tests\Mock\BeforeAndAfterTest;
use Zynga\PHPUnit\V2\Tests\Mock\BeforeClassAndAfterClassTest;
use Zynga\PHPUnit\V2\Tests\Mock\BeforeClassWithOnlyDataProviderTest;
use Zynga\PHPUnit\V2\Tests\Mock\DataProviderIncompleteTest;
use Zynga\PHPUnit\V2\Tests\Mock\InheritedTestCase;
use Zynga\PHPUnit\V2\Tests\Mock\OneTestCase;
use Zynga\PHPUnit\V2\Tests\Mock\NoTestCases;
use Zynga\PHPUnit\V2\Tests\Mock\NoTestCaseClass;
use Zynga\PHPUnit\V2\Tests\Mock\RequirementsClassBeforeClassHookTest;
use Zynga\PHPUnit\V2\Tests\Mock\NotVoidTestCase;
use Zynga\PHPUnit\V2\Tests\Mock\NotPublicTestCase;
use Zynga\PHPUnit\V2\Tests\Mock\OverrideTestCase;

/**
 * @since      Class available since Release 2.0.0
 * @covers     PHPUnit_Framework_TestSuite
 */
class SuiteTest extends BaseTest {

  public function testAddTestSuite(): void {

    $suite = new TestSuite(OneTestCase::class);

    $result = $suite->run();

    $this->_verifyTestSuite(
      $suite,
      $result,
      false, // debug
      Status::STATUS_PASSED, // status
      1, // testCount
      1, // successful
      0, // error
      0, // failure
      0, // skipped
      0, // incomplete
      0 // warning
    );

  }

  public function testInheritedTests(): void {

    $suite = new TestSuite(InheritedTestCase::class);

    $result = $suite->run();

    $this->_verifyTestSuite(
      $suite,
      $result,
      false, // debug
      Status::STATUS_PASSED, // status
      2, // testCount
      2, // successful
      0, // error
      0, // failure
      0, // skipped
      0, // incomplete
      0 // warning
    );

  }

  public function testNoTestCases(): void {

    $suite = new TestSuite(NoTestCases::class);

    $result = $suite->run();

    $this->_verifyTestSuite(
      $suite,
      $result,
      false, // debug
      Status::STATUS_WARNING, // status
      1, // testCount
      0, // successful
      0, // error
      0, // failure
      0, // skipped
      0, // incomplete
      1 // warning
    );

  }

  public function testNoTestCaseClass(): void {

    $suite = new TestSuite(NoTestCaseClass::class);

    $initialResult = $suite->getResult();

    $this->_verifyTestSuite(
      $suite,
      $initialResult,
      false, // debug
      Status::STATUS_FAILURE, // status
      0, // testCount
      0, // successful
      1, // error
      0, // failure
      0, // skipped
      0, // incomplete
      0, // warning
      0, // notImplemented
    );

  }

  public function testNotExistingTestCase(): void {

    $suite = new TestSuite('NotExistingTestCase');

    $initialResult = $suite->getResult();

    $this->_verifyTestSuite(
      $suite,
      $initialResult,
      false, // debug
      Status::STATUS_FAILURE, // status
      0, // testCount
      0, // successful
      1, // error
      0, // failure
      0, // skipped
      0, // incomplete
      0, // warning
      0, // notImplemented
    );

    $result = $suite->run();

    $this->_verifyTestSuite(
      $suite,
      $result,
      false, // debug
      Status::STATUS_FAILURE, // status
      0, // testCount
      0, // successful
      1, // error
      0, // failure
      0, // skipped
      0, // incomplete
      0, // warning
      0, // notImplemented
    );

  }

  public function testNotPublicTestCase(): void {

    $suite = new TestSuite(NotPublicTestCase::class);

    $result = $suite->getResult();

    $this->_verifyTestSuite(
      $suite,
      $result,
      false, // debug
      Status::STATUS_NOT_STARTED, // status
      1, // testCount
      0, // successful
      0, // error
      0, // failure
      0, // skipped
      0, // incomplete
      2, // warning
      0, // notImplemented
    );

  }

  public function testNotVoidTestCase(): void {

    $suite = new TestSuite(NotVoidTestCase::class);
    $result = new TestResult();

    $this->_verifyTestSuite(
      $suite,
      $result,
      false, // debug
      Status::STATUS_NOT_STARTED, // status
      1, // testCount
      0, // successful
      0, // error
      0, // failure
      0, // skipped
      0, // incomplete
      0, // warning
      0, // notImplemented
    );

  }

  public function testOneTestCase(): void {

    $suite = new TestSuite(OneTestCase::class);

    $result = $suite->run();

    $this->_verifyTestSuite(
      $suite,
      $result,
      false, // debug
      Status::STATUS_PASSED, // status
      1, // testCount
      1, // successful
      0, // error
      0, // failure
      0, // skipped
      0, // incomplete
      0, // warning
      0, // notImplemented
    );

  }

  public function testShadowedTests(): void {

    $suite = new TestSuite(OverrideTestCase::class);

    $result = $suite->run();

    $this->_verifyTestSuite(
      $suite,
      $result,
      false, // debug
      Status::STATUS_PASSED, // status
      1, // testCount
      1, // successful
      0, // error
      0, // failure
      0, // skipped
      0, // incomplete
      0, // warning
      0, // notImplemented
    );

  }

  public function testBeforeClassAndAfterClassAnnotations(): void {

    $suite = new TestSuite(BeforeClassAndAfterClassTest::class);

    BeforeClassAndAfterClassTest::resetProperties();

    $result = $suite->run();

    $this->assertEquals(
      1,
      BeforeClassAndAfterClassTest::$beforeClassWasRun,
      'beforeClass method was not run once for the whole suite.',
    );

    $this->assertEquals(
      1,
      BeforeClassAndAfterClassTest::$afterClassWasRun,
      'afterClass method was not run once for the whole suite.',
    );

  }

  public function testBeforeClassWithDataProviders(): void {

    $suite = new TestSuite('BeforeClassWithOnlyDataProviderTest');

    BeforeClassWithOnlyDataProviderTest::resetProperties();

    $result = $suite->run();

    $this->assertTrue(
      BeforeClassWithOnlyDataProviderTest::$setUpBeforeClassWasCalled,
      'setUpBeforeClass method was not run.',
    );

    $this->assertTrue(
      BeforeClassWithOnlyDataProviderTest::$beforeClassWasCalled,
      '@beforeClass method was not run.',
    );

  }

  public function testBeforeAnnotation(): void {

    $test = new TestSuite('BeforeAndAfterTest');

    BeforeAndAfterTest::resetProperties();

    $result = $test->run();

    $this->assertEquals(2, BeforeAndAfterTest::$beforeWasRun);
    $this->assertEquals(2, BeforeAndAfterTest::$afterWasRun);

  }

  public function testTestWithAnnotation(): void {

    $suite = new TestSuite('TestWithTest');

    BeforeAndAfterTest::resetProperties();

    $result = $suite->run();

    $this->_verifyTestSuite(
      $suite,
      $result,
      false, // debug
      Status::STATUS_PASSED, // status
      4, // testCount
      4, // successful
      0, // error
      0, // failure
      0, // skipped
      0, // incomplete
      0, // warning
      0, // notImplemented
    );

  }

  public function testSkippedTestDataProvider(): void {

    $suite = new TestSuite('DataProviderSkippedTest');

    $result = $suite->run();

    $this->_verifyTestSuite(
      $suite,
      $result,
      false, // debug
      Status::STATUS_PASSED, // status
      1, // testCount
      0, // successful
      0, // error
      0, // failure
      1, // skipped
      0, // incomplete
      0, // warning
      0, // notImplemented
    );

  }

  public function testIncompleteTestDataProvider(): void {

    $suite = new TestSuite(DataProviderIncompleteTest::class);

    $result = $suite->run();

    $this->_verifyTestSuite(
      $suite,
      $result,
      false, // debug
      Status::STATUS_PASSED, // status
      3, // testCount
      3, // successful
      0, // error
      0, // failure
      0, // skipped
      1, // incomplete
      0, // warning
      0, // notImplemented
    );

  }

  public function testRequirementsBeforeClassHook(): void {

    $suite = new TestSuite(RequirementsClassBeforeClassHookTest::class);

    $result = $suite->run();

    $this->_verifyTestSuite(
      $suite,
      $result,
      false, // debug
      Status::STATUS_PASSED, // status
      1, // testCount
      0, // successful
      0, // error
      0, // failure
      0, // skipped
      0, // incomplete
      1, // warning
      0, // notImplemented
    );

  }

  public function testDontSkipInheritedClass(): void {

    // This isn't a class ref, it's a string label for a 'virtual' test.
    $suite = new TestSuite('DontSkipInheritedClass');

    // --
    // JEO: Purposely take a hard path to the tests in order to test the flow around, finding files and
    // adding tests from the file ssystem.
    // --
    $suite->addTestFile(
      $this->getTestSourcePath().
      DIRECTORY_SEPARATOR.
      'Mock'.
      DIRECTORY_SEPARATOR.
      'Inheritence'.
      DIRECTORY_SEPARATOR.
      'InheritanceA.hh',
    );

    $suite->addTestFile(
      $this->getTestSourcePath().
      DIRECTORY_SEPARATOR.
      'Mock'.
      DIRECTORY_SEPARATOR.
      'Inheritence'.
      DIRECTORY_SEPARATOR.
      'InheritanceB.hh',
    );

    $result = $suite->run();

    $this->_verifyTestSuite(
      $suite,
      $result,
      false, // debug
      Status::STATUS_PASSED, // status
      2, // testCount
      2, // successful
      0, // error
      0, // failure
      0, // skipped
      0, // incomplete
      0, // warning
      0, // notImplemented
    );

  }

}
