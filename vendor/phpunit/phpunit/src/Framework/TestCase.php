<?php
/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use SebastianBergmann\GlobalState\Snapshot;
use SebastianBergmann\GlobalState\Restorer;
use SebastianBergmann\GlobalState\Blacklist;
use SebastianBergmann\Diff\Differ;
use SebastianBergmann\Exporter\Exporter;
use SebastianBergmann\ObjectEnumerator\Enumerator;
use SebastianBergmann\PHPUnit\Environment\OutputBuffering;
use SebastianBergmann\PHPUnit\Exceptions\AssertionFailedException;
use SebastianBergmann\PHPUnit\Exceptions\TestError\IncompleteException;
use SebastianBergmann\PHPUnit\Exceptions\TestError\SkippedException;
use Prophecy\Exception\Prediction\PredictionException;
use Prophecy\Prophet;
use DeepCopy\DeepCopy;
use Zynga\Framework\ReflectionCache\V1\ReflectionClasses;
use Zynga\PHPUnit\V2\TestCase;

use \Throwable;
use \Exception;

/**
 * A TestCase defines the fixture to run multiple tests.
 *
 * To define a TestCase
 *
 *   1) Implement a subclass of PHPUnit_Framework_TestCase.
 *   2) Define instance variables that store the state of the fixture.
 *   3) Initialize the fixture state by overriding setUp().
 *   4) Clean-up after a test by overriding tearDown().
 *
 * Each test runs in its own fixture so there can be no side effects
 * among test runs.
 *
 * Here is an example:
 *
 * <code>
 * <?php
 * class MathTest extends PHPUnit_Framework_TestCase
 * {
 *     public $value1;
 *     public $value2;
 *
 *     protected function setUp()
 *     {
 *         $this->value1 = 2;
 *         $this->value2 = 3;
 *     }
 * }
 * ?>
 * </code>
 *
 * For each test implement a method which interacts with the fixture.
 * Verify the expected results with assertions specified by calling
 * assert with a boolean.
 *
 * <code>
 * <?php
 * public function testPass()
 * {
 *     $this->assertTrue($this->value1 + $this->value2 == 5);
 * }
 * ?>
 * </code>
 *
 * @since Class available since Release 2.0.0
 */
abstract class PHPUnit_Framework_TestCase extends TestCase implements PHPUnit_Framework_Test, PHPUnit_Framework_SelfDescribing
{

  private PHPUnit_Framework_Test $_test = null;

    /**
     * Enable or disable the backup and restoration of the $GLOBALS array.
     * Overwrite this attribute in a child class of TestCase.
     * Setting this attribute in setUp() has no effect!
     *
     * @var bool
     */
    protected $backupGlobals = null;

    /**
     * @var array
     */
    protected $backupGlobalsBlacklist = [];

    /**
     * Enable or disable the backup and restoration of static attributes.
     * Overwrite this attribute in a child class of TestCase.
     * Setting this attribute in setUp() has no effect!
     *
     * @var bool
     */
    protected $backupStaticAttributes = null;

    /**
     * @var array
     */
    protected $backupStaticAttributesBlacklist = [];

    /**
     * Whether or not this test is to be run in a separate PHP process.
     *
     * @var bool
     */
    protected $runTestInSeparateProcess = null;

    /**
     * Whether or not this test should preserve the global state when
     * running in a separate PHP process.
     *
     * @var bool
     */
    protected $preserveGlobalState = false; // JEO: Changed this to default to off


    /**
     * Whether or not this test is running in a separate PHP process.
     *
     * @var bool
     */
    private $inIsolation = false;

    /**
     * @var array
     */
    private $data = [];

    /**
     * @var string
     */
    private $dataName = '';

    /**
     * @var bool
     */
    private $useErrorHandler = null;

    /**
     * The name of the expected Exception.
     *
     * @var string
     */
    private $expectedException = '';

    /**
     * The message of the expected Exception.
     *
     * @var string
     */
    private $expectedExceptionMessage = '';

    /**
     * The regex pattern to validate the expected Exception message.
     *
     * @var string
     */
    private $expectedExceptionMessageRegExp = '';

    /**
     * The code of the expected Exception
     *
     * @var int
     */
    // @TODO: JEO we should work on this not using null as a value.
    private $expectedExceptionCode = null;

    /**
     * The name of the test case.
     *
     * @var string
     */
    private $name = null;

    /**
     * @var array
     */
    private $dependencies = [];

    /**
     * @var array
     */
    private $dependencyInput = [];

    /**
     * @var array
     */
    private $iniSettings = [];

    /**
     * @var array
     */
    private $locale = [];

    /**
     * @var array
     */
    //private $mockObjects = [];

    /**
     * @var array
     */
    //private $mockObjectGenerator = null;

    /**
     * @var int
     */
    private $numAssertions = 0;

    /**
     * @var PHPUnit_Framework_TestResult
     */
    private $result;

    /**
     * @var mixed
     */
    private $testResult;

    /**
     * @var string
     */
    private $output = '';

    /**
     * @var string
     */
    private $outputExpectedRegex = null;

    /**
     * @var string
     */
    private $outputExpectedString = null;

    /**
     * @var mixed
     */
    private $outputCallback = false;

    /**
     * @var int
     */
    private $outputBufferingLevel;

    /**
     * @var SebastianBergmann\GlobalState\Snapshot
     */
    private $snapshot;

    /**
     * @var Prophecy\Prophet
     */
    private $prophet;

    /**
     * @var bool
     */
    private $beStrictAboutChangesToGlobalState = false;

    /**
     * @var bool
     */
    private $registerMockObjectsFromTestArgumentsRecursively = false;

    /**
     * @var string[]
     */
    private $warnings = [];

    /**
     * @var array
     */
    private $groups = [];

    /**
     * Constructs a test case with the given name.
     *
     * @param string $name
     * @param array  $data
     * @param string $dataName
     */
    public function __construct($name = null, array $data = [], $dataName = '')
    {

      // call the hack strict testcase constructor.
      parent::__construct();

        if ($name !== null) {
            $this->setName($name);
        }

        $this->data     = $data;
        $this->dataName = $dataName;
    }


    public function setTest(PHPUnit_Framework_Test $test) {
      $this->_test = $test;
      return true;
    }

    public function getTest(): PHPUnit_Framework_Test {
      if ( $this->_test === null ) {
        $this->_test = $this;
      }
      return $this->_test;
    }

    /**
     * Returns a string representation of the test case.
     *
     * @return string
     */
    public function toString()
    {
      $test = $this->getTest();

        $class = ReflectionClasses::getReflection($test);

        $buffer = sprintf(
            '%s::%s',
            $class->name,
            $test->getName(false)
        );

        return $buffer . $this->getDataSetAsString();
    }

    /**
     * Counts the number of test cases executed by run(TestResult result).
     *
     * @return int
     */
    public function count()
    {
        return 1;
    }

    /**
     * @since Method available since Release 5.4.0
     */
    public function getGroups()
    {
        return $this->groups;
    }

    /**
     * @param array $groups
     *
     * @since Method available since Release 5.4.0
     */
    public function setGroups(array $groups)
    {
        $this->groups = $groups;
    }

    /**
     * Returns the annotations for this test.
     *
     * @return array
     *
     * @since Method available since Release 3.4.0
     */
    public function getAnnotations()
    {
      $test = $this->getTest();
        return PHPUnit_Util_Test::parseTestMethodAnnotations(
            get_class($test),
            $test->getName(false)
        );
    }

    /**
     * Gets the name of a TestCase.
     *
     * @param bool $withDataSet
     *
     * @return string
     */
    public function getName($withDataSet = true)
    {
        if ($withDataSet) {
            return $this->name . $this->getDataSetAsString(false);
        } else {
            return $this->name;
        }
    }

    /**
     * Returns the size of the test.
     *
     * @return int
     *
     * @since Method available since Release 3.6.0
     */
    public function getSize()
    {
          return PHPUnit_Util_Test::getSize(
            get_class($this->getTest()),
            $this->getName(false)
        );
    }

    /**
     * @return bool
     *
     * @since Method available since Release 5.3.4
     */
    public function hasSize()
    {
        return $this->getSize() !== PHPUnit_Util_Test::UNKNOWN;
    }

    /**
     * @return bool
     *
     * @since Method available since Release 5.3.4
     */
    public function isSmall()
    {
        return $this->getSize() === PHPUnit_Util_Test::SMALL;
    }

    /**
     * @return bool
     *
     * @since Method available since Release 5.3.4
     */
    public function isMedium()
    {
        return $this->getSize() === PHPUnit_Util_Test::MEDIUM;
    }

    /**
     * @return bool
     *
     * @since Method available since Release 5.3.4
     */
    public function isLarge()
    {
        return $this->getSize() === PHPUnit_Util_Test::LARGE;
    }

    /**
     * @return string
     *
     * @since Method available since Release 3.6.0
     */
    public function getActualOutput()
    {
        if (! OutputBuffering::getIsActive() ) {
            return $this->output;
        } else {
            return OutputBuffering::get();
        }
    }

    /**
     * @return bool
     *
     * @since Method available since Release 3.6.0
     */
    public function hasOutput()
    {
        if (strlen($this->output) === 0) {
            return false;
        }

        if ($this->hasExpectationOnOutput()) {
            return false;
        }

        return true;
    }

    /**
     * @param string $expectedRegex
     *
     * @since Method available since Release 3.6.0
     *
     * @throws PHPUnit_Framework_Exception
     */
    public function expectOutputRegex($expectedRegex)
    {
        if ($this->outputExpectedString !== null) {
            throw new PHPUnit_Framework_Exception;
        }

        if (is_string($expectedRegex) || is_null($expectedRegex)) {
            $this->outputExpectedRegex = $expectedRegex;
        }
    }

    /**
     * @param string $expectedString
     *
     * @since Method available since Release 3.6.0
     */
    public function expectOutputString($expectedString)
    {
        if ($this->outputExpectedRegex !== null) {
            throw new PHPUnit_Framework_Exception;
        }

        if (is_string($expectedString) || is_null($expectedString)) {
            $this->outputExpectedString = $expectedString;
        }
    }

    /**
     * @return bool
     *
     * @since Method available since Release 3.6.5
     * @deprecated
     */
    public function hasPerformedExpectationsOnOutput()
    {
        return $this->hasExpectationOnOutput();
    }

    /**
     * @return bool
     *
     * @since Method available since Release 4.3.3
     */
    public function hasExpectationOnOutput()
    {
        return is_string($this->outputExpectedString) || is_string($this->outputExpectedRegex);
    }

    /**
     * @return string
     *
     * @since Method available since Release 3.2.0
     */
    public function getExpectedException()
    {
        return $this->expectedException;
    }

    /**
     * @param mixed      $exception
     * @param string     $message
     * @param int|string $code
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since      Method available since Release 3.2.0
     * @deprecated Method deprecated since Release 5.2.0
     */
    public function setExpectedException($exception, $message = '', $code = null)
    {
        $this->expectedException = $exception;

        if ($message !== null && $message !== '') {
            $this->expectExceptionMessage($message);
        }

        if ($code !== null) {
            $this->expectExceptionCode($code);
        }
    }

    /**
     * @param mixed  $exception
     * @param string $messageRegExp
     * @param int    $code
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 4.3.0
     */
    public function setExpectedExceptionRegExp($exception, $messageRegExp = '', $code = null)
    {
        if (!is_string($messageRegExp)) {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(2, 'string');
        }

        $this->expectedException              = $exception;
        $this->expectedExceptionMessageRegExp = $messageRegExp;

        if ($code !== null) {
            $this->expectExceptionCode($code);
        }
    }

    /**
     * @param mixed $exception
     *
     * @since Method available since Release 5.2.0
     */
    public function expectException($exception)
    {
        $this->expectedException = $exception;
    }

    /**
     * @param int|string $code
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 5.2.0
     */
    public function expectExceptionCode($code)
    {
        if (!is_int($code) && !is_string($code)) {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'integer or string');
        }

        $this->expectedExceptionCode = $code;
    }

    /**
     * @param string $message
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 5.2.0
     */
    public function expectExceptionMessage($message)
    {
        if (!is_string($message)) {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'string');
        }

        $this->expectedExceptionMessage = $message;
    }

    /**
     * @param string $messageRegExp
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 5.2.0
     */
    public function expectExceptionMessageRegExp($messageRegExp)
    {
        if (!is_string($messageRegExp)) {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'string');
        }

        $this->expectedExceptionMessageRegExp = $messageRegExp;
    }

    /**
     * @param bool $flag
     *
     * @since Method available since Release 5.4.0
     */
    public function setRegisterMockObjectsFromTestArgumentsRecursively($flag)
    {
        if (!is_bool($flag)) {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'boolean');
        }

        $this->registerMockObjectsFromTestArgumentsRecursively = $flag;
    }

    /**
     * @since Method available since Release 3.4.0
     */
    protected function setExpectedExceptionFromAnnotation()
    {
        try {

          $test = $this->getTest();

            $expectedException = PHPUnit_Util_Test::getExpectedException(
                get_class($test),
                $test->getName(false)
            );

            if ($expectedException !== false) {
                $this->expectException($expectedException['class']);

                if ($expectedException['code'] !== null) {
                    $this->expectExceptionCode($expectedException['code']);
                }

                if ($expectedException['message'] !== '') {
                    $this->expectExceptionMessage($expectedException['message']);
                } elseif ($expectedException['message_regex'] !== '') {
                    $this->expectExceptionMessageRegExp($expectedException['message_regex']);
                }
            }
        } catch (ReflectionException $e) {
        }
    }

    /**
     * @param bool $useErrorHandler
     *
     * @since Method available since Release 3.4.0
     */
    public function setUseErrorHandler($useErrorHandler)
    {
        $this->useErrorHandler = $useErrorHandler;
    }

    /**
     * @since Method available since Release 3.4.0
     */
    protected function setUseErrorHandlerFromAnnotation()
    {
        try {
          $test = $this->getTest();

          $useErrorHandler = PHPUnit_Util_Test::getErrorHandlerSettings(
                get_class($test),
                $test->getName(false)
            );

            if ($useErrorHandler !== null) {
                $this->setUseErrorHandler($useErrorHandler);
            }
        } catch (ReflectionException $e) {
        }
    }

    /**
     * @since Method available since Release 3.6.0
     */
    protected function checkRequirements()
    {
      $test = $this->getTest();
      $name = $test->getName(false);
        if (! $name || !method_exists($test, $name)) {
            return;
        }

        $missingRequirements = PHPUnit_Util_Test::getMissingRequirements(
            get_class($test),
            $name
        );

        if (count($missingRequirements) > 0) {
            $this->markTestSkipped(implode(PHP_EOL, $missingRequirements));
        }
    }

    /**
     * Returns the status of this test.
     *
     * @return int
     *
     * @since Method available since Release 3.1.0
     */
    public function getStatus()
    {
        return $this->status()->getCode();
    }

    /**
     * Returns the status message of this test.
     *
     * @return string
     *
     * @since Method available since Release 3.3.0
     */
    public function getStatusMessage() {
      //var_dump(__LINE__ . ' getStatusMessage: ' . $this->status()->getMessage());
      return $this->status()->getMessage();
    }

    /**
     * Returns whether or not this test has failed.
     *
     * @return bool
     *
     * @since Method available since Release 3.0.0
     */
    public function hasFailed()
    {
        $status = $this->getStatus();

        return $status == PHPUnit_Runner_BaseTestRunner::STATUS_FAILURE ||
               $status == PHPUnit_Runner_BaseTestRunner::STATUS_ERROR;
    }

    /**
     * Runs the test case and collects the results in a TestResult object.
     * If no TestResult object is passed a new one will be created.
     *
     * @param PHPUnit_Framework_TestResult $result
     * @param PHPUnit_Framework_Test $object
     *
     * @return PHPUnit_Framework_TestResult
     *
     * @throws PHPUnit_Framework_Exception
     */
    public function run(PHPUnit_Framework_TestResult $result = null)
    {

      $test = $this->getTest();

        if ($result === null) {
            $result = $this->createResult();
        }

        if (!$this instanceof PHPUnit_Framework_WarningTestCase) {
            $this->setTestResultObject($result);
            $this->setUseErrorHandlerFromAnnotation($test);
        }

        if ($this->useErrorHandler !== null) {
            $oldErrorHandlerSetting = $result->getConvertErrorsToExceptions();
            $result->convertErrorsToExceptions($this->useErrorHandler);
        }

        if (!$this instanceof PHPUnit_Framework_WarningTestCase && !$this->handleDependencies()) {
            return;
        }

        // Pass on the test into the run function for result.
        $result->run($test);

        if ($this->useErrorHandler !== null) {
            $result->convertErrorsToExceptions($oldErrorHandlerSetting);
        }

        $this->result = null;

        return $result;
    }

    /**
     * Runs the bare test sequence.
     */
    public function runBare()
    {
      $test = $this->getTest();

        $this->numAssertions = 0;

        $this->snapshotGlobalState();
        $this->startOutputBuffering();
        clearstatcache();
        $currentWorkingDirectory = getcwd();

        $hookMethods = PHPUnit_Util_Test::getHookMethods(get_class($test));

        try {
            $hasMetRequirements = false;
            $this->checkRequirements();
            $hasMetRequirements = true;
            if ($this->inIsolation) {
                foreach ($hookMethods['beforeClass'] as $method) {
                    $test->$method();
                }
            }

            $this->setExpectedExceptionFromAnnotation();

            foreach ($hookMethods['before'] as $method) {
              if ( method_exists($test, $method) ) {
                $test->$method();
              }
            }

            $this->assertPreConditions();
            $this->testResult = $this->runTest($test);
            //$this->verifyMockObjects();
            $this->assertPostConditions();

            if (!empty($this->warnings)) {
                throw new PHPUnit_Framework_Warning(
                    implode(
                        "\n",
                        array_unique($this->warnings)
                    )
                );
            }

          $this->status()->setMessageAndCode(
            '',
            PHPUnit_Runner_BaseTestRunner::STATUS_PASSED
          );

        } catch (IncompleteException $e) {
          $this->status()->setMessageAndCode(
            $e->getMessage(),
            PHPUnit_Runner_BaseTestRunner::STATUS_INCOMPLETE
          );
        } catch (PHPUnit_Framework_IncompleteTest $e) {
          $this->status()->setMessageAndCode(
            $e->getMessage(),
            PHPUnit_Runner_BaseTestRunner::STATUS_INCOMPLETE
          );
        } catch (SkippedException $e) {
          $this->status()->setMessageAndCode(
            $e->getMessage(),
            PHPUnit_Runner_BaseTestRunner::STATUS_SKIPPED
          );
        } catch (PHPUnit_Framework_SkippedTest $e) {
          $this->status()->setMessageAndCode(
            $e->getMessage(),
            PHPUnit_Runner_BaseTestRunner::STATUS_SKIPPED
          );
        } catch (PHPUnit_Framework_Warning $e) {
          $this->status()->setMessageAndCode(
            $e->getMessage(),
            PHPUnit_Runner_BaseTestRunner::STATUS_WARNING
          );
        } catch (AssertionFailedException $e) {
          $this->status()->setMessageAndCode(
            $e->getMessage(),
            PHPUnit_Runner_BaseTestRunner::STATUS_FAILURE
          );
        } catch (PHPUnit_Framework_AssertionFailedError $e) {
          $this->status()->setMessageAndCode(
            $e->getMessage(),
            PHPUnit_Runner_BaseTestRunner::STATUS_FAILURE
          );
        } catch (PredictionException $e) {
          // var_dump('JEO PredictionException triggered ' . get_class($_e));
          // var_dump($e->getMessage());
          $this->status()->setMessageAndCode(
            $e->getMessage(),
            PHPUnit_Runner_BaseTestRunner::STATUS_FAILURE
          );
        } catch (Throwable $_e) {
          $e = $_e;
        } catch (Exception $_e) {
          $e = $_e;
        }

        if (isset($_e)) {
          $this->status()->setMessageAndCode(
            $_e->getMessage(),
            PHPUnit_Runner_BaseTestRunner::STATUS_ERROR
          );
        }

        // Clean up the mock objects.
        //$this->mockObjects = [];
        $this->prophet     = null;

        // Tear down the fixture. An exception raised in tearDown() will be
        // caught and passed on when no exception was raised before.
        try {
            if ($hasMetRequirements) {
                foreach ($hookMethods['after'] as $method) {
                    $test->$method();
                }

                if ($this->inIsolation) {
                    foreach ($hookMethods['afterClass'] as $method) {
                        $test->$method();
                    }
                }
            }
        } catch (Throwable $_e) {
            if (!isset($e)) {
                $e = $_e;
            }
        } catch (Exception $_e) {
            if (!isset($e)) {
                $e = $_e;
            }
        }

        try {
            $this->stopOutputBuffering();
        } catch (PHPUnit_Framework_RiskyTestError $_e) {
            if (!isset($e)) {
                $e = $_e;
            }
        }

        clearstatcache();

        if ($currentWorkingDirectory != getcwd()) {
            chdir($currentWorkingDirectory);
        }

        $this->restoreGlobalState();

        // Clean up INI settings.
        foreach ($this->iniSettings as $varName => $oldValue) {
            ini_set($varName, $oldValue);
        }

        $this->iniSettings = [];

        // Clean up locale settings.
        foreach ($this->locale as $category => $locale) {
            setlocale($category, $locale);
        }

        // Perform assertion on output.
        if (!isset($e)) {
            try {
                if ($this->outputExpectedRegex !== null) {
                    $this->assertRegExp($this->outputExpectedRegex, $this->output);
                } elseif ($this->outputExpectedString !== null) {
                    $this->assertEquals($this->outputExpectedString, $this->output);
                }
            } catch (Throwable $_e) {
                $e = $_e;
            } catch (Exception $_e) {
                $e = $_e;
            }
        }

        // Workaround for missing "finally".
        if (isset($e)) {
            if ($e instanceof PredictionException) {
                $e = new PHPUnit_Framework_AssertionFailedError($e->getMessage());
            }

            $this->onNotSuccessfulTest($e);
        }
    }

    /**
     * Override to run the test and assert its state.
     *
     * @return mixed
     *
     * @throws Exception|PHPUnit_Framework_Exception
     * @throws PHPUnit_Framework_Exception
     */
    protected function runTest()    {

      $test = $this->getTest();

      $testName = $test->getName(false);

        if ($testName === null) {
            throw new PHPUnit_Framework_Exception(
                'PHPUnit_Framework_TestCase::$name must not be null.'
            );
        }

        try {
            $class  = ReflectionClasses::getReflection($test);
            $method = $class->getMethod($testName);
        } catch (ReflectionException $e) {
            $this->fail($e->getMessage());
        }

        $testArguments = array_merge($this->data, $this->dependencyInput);

        $this->registerMockObjectsFromTestArguments($testArguments);

        $e = null;
        $testResult = null;

        try {
          $testResult = $method->invokeArgs($test, $testArguments);
        } catch (Throwable $_e) {
            $e = $_e;
        } catch (Exception $_e) {
            $e = $_e;
        }


        $checkException = false;

        if ( $this->expectedException != '' ) {
          $checkException = true;
        }

        // var_dump(get_class($test));
        // var_dump($testName);
        // var_dump($this->expectedException);
        // var_dump($checkException);
        // exit();

        // --
        // JEO: I am pretty sure we don't need the following code anymore after
        // the refactors to remove the overloaded Exception usage.
        // --
        // if ($e instanceof Exception) {
        //
        //     if ($e instanceof PHPUnit_Framework_Exception) {
        //         $checkException = false;
        //     }
        //
        //     $reflector = ReflectionClasses::getReflection($this->expectedException);
        //
        //     if ($reflector != null && ($this->expectedException == 'PHPUnit_Framework_Exception' ||
        //       $reflector->isSubclassOf('PHPUnit_Framework_Exception'))) {
        //       $checkException = true;
        //     }
        //
        // }

        if ($checkException == true) {

          try {


            $this->assertExceptionSameClass($this->expectedException, $e);

            if ($this->expectedExceptionMessage != '') {
              $this->assertExceptionSameMessage($this->expectedExceptionMessage, $e);
            }

            if ($this->expectedExceptionMessageRegExp != '') {
              $this->assertExceptionRegexpMessage($this->expectedExceptionMessageRegExp, $e);
            }

            if ($this->expectedExceptionCode !== null) {
              $this->assertEquals(
                $this->expectedExceptionCode,
                $e->getCode()
              );
            }

          } catch (AssertionFailedException $expectedError) {

            $this->fail($expectedError->getMessage());

          }

          return;

      } elseif ( $e instanceof Throwable ){
        // So we have a non-monitored exception, re-throw.
        throw $e;
      } elseif ( $e instanceof Exception ) {
        // So we have a non-monitored exception, re-throw.
        throw $e;
      }

      return $testResult;

    }

    /**
     * Verifies the mock object expectations.
     *
     * @since Method available since Release 3.5.0
     */
    protected function verifyMockObjects() {
      throw new Exception('verifyMockObject is deprecated in the hacklang version');
    }
    // {
    //     foreach ($this->mockObjects as $mockObject) {
    //         if ($mockObject->__phpunit_hasMatchers()) {
    //             $this->numAssertions++;
    //         }
    //
    //         $mockObject->__phpunit_verify(
    //             $this->shouldInvocationMockerBeReset($mockObject)
    //         );
    //     }
    //
    //     if ($this->prophet !== null) {
    //         try {
    //             $this->prophet->checkPredictions();
    //         } catch (Throwable $t) {
    //             /* Intentionally left empty */
    //         } catch (Exception $t) {
    //             /* Intentionally left empty */
    //         }
    //
    //         foreach ($this->prophet->getProphecies() as $objectProphecy) {
    //             foreach ($objectProphecy->getMethodProphecies() as $methodProphecies) {
    //                 foreach ($methodProphecies as $methodProphecy) {
    //                     $this->numAssertions += count($methodProphecy->getCheckedPredictions());
    //                 }
    //             }
    //         }
    //
    //         if (isset($t)) {
    //             throw $t;
    //         }
    //     }
    // }

    /**
     * Sets the name of a TestCase.
     *
     * @param  string
     */
    public function setName($name)
    {
        $this->name = $name;
    }

    /**
     * Sets the dependencies of a TestCase.
     *
     * @param array $dependencies
     *
     * @since Method available since Release 3.4.0
     */
    public function setDependencies(array $dependencies)
    {
        $this->dependencies = $dependencies;
    }

    /**
     * Returns true if the tests has dependencies
     *
     * @return bool
     *
     * @since Method available since Release 4.0.0
     */
    public function hasDependencies()
    {
        return count($this->dependencies) > 0;
    }

    /**
     * Sets
     *
     * @param array $dependencyInput
     *
     * @since Method available since Release 3.4.0
     */
    public function setDependencyInput(array $dependencyInput)
    {
        $this->dependencyInput = $dependencyInput;
    }

    /**
     * @param bool $beStrictAboutChangesToGlobalState
     *
     * @since Method available since Release 4.6.0
     */
    public function setBeStrictAboutChangesToGlobalState($beStrictAboutChangesToGlobalState)
    {
        $this->beStrictAboutChangesToGlobalState = $beStrictAboutChangesToGlobalState;
    }

    /**
     * Calling this method in setUp() has no effect!
     *
     * @param bool $backupGlobals
     *
     * @since Method available since Release 3.3.0
     */
    public function setBackupGlobals($backupGlobals)
    {
        if (is_null($this->backupGlobals) && is_bool($backupGlobals)) {
            $this->backupGlobals = $backupGlobals;
        }
    }

    /**
     * Calling this method in setUp() has no effect!
     *
     * @param bool $backupStaticAttributes
     *
     * @since Method available since Release 3.4.0
     */
    public function setBackupStaticAttributes($backupStaticAttributes)
    {
        if (is_null($this->backupStaticAttributes) &&
            is_bool($backupStaticAttributes)) {
            $this->backupStaticAttributes = $backupStaticAttributes;
        }
    }

    /**
     * @param bool $runTestInSeparateProcess
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 3.4.0
     */
    public function setRunTestInSeparateProcess($runTestInSeparateProcess)
    {
        if (is_bool($runTestInSeparateProcess)) {
            if ($this->runTestInSeparateProcess === null) {
                $this->runTestInSeparateProcess = $runTestInSeparateProcess;
            }
        } else {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'boolean');
        }
    }

    /**
     * @param bool $preserveGlobalState
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 3.4.0
     */
    public function setPreserveGlobalState($preserveGlobalState)
    {
      $this->preserveGlobalState = false;
      /*
        if (is_bool($preserveGlobalState)) {
            $this->preserveGlobalState = $preserveGlobalState;
        } else {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'boolean');
        }
       */
    }

    /**
     * @param bool $inIsolation
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 3.4.0
     */
    public function setInIsolation($inIsolation)
    {
        if (is_bool($inIsolation)) {
            $this->inIsolation = $inIsolation;
        } else {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'boolean');
        }
    }

    /**
     * @return bool
     *
     * @since Method available since Release 4.3.0
     */
    public function isInIsolation()
    {
        return $this->inIsolation;
    }

    /**
     * @return mixed
     *
     * @since Method available since Release 3.4.0
     */
    public function getResult()
    {
        return $this->testResult;
    }

    /**
     * @param mixed $result
     *
     * @since Method available since Release 3.4.0
     */
    public function setResult($result)
    {
        $this->testResult = $result;
    }

    /**
     * @param callable $callback
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 3.6.0
     */
    public function setOutputCallback($callback)
    {
        if (!is_callable($callback)) {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'callback');
        }

        $this->outputCallback = $callback;
    }

    /**
     * @return PHPUnit_Framework_TestResult
     *
     * @since Method available since Release 3.5.7
     */
    public function getTestResultObject()
    {
        return $this->result;
    }

    /**
     * @param PHPUnit_Framework_TestResult $result
     *
     * @since Method available since Release 3.6.0
     */
    public function setTestResultObject(PHPUnit_Framework_TestResult $result)
    {
        $this->result = $result;
    }

    /**
     * @param PHPUnit_Framework_MockObject_MockObject $mockObject
     *
     * @since Method available since Release 5.4.0
     */
    public function registerMockObject(PHPUnit_Framework_MockObject_MockObject $mockObject)
    {
      throw new Exception('registerMockObject is deprecated in the hacklang version');
        // $this->mockObjects[] = $mockObject;
    }

    /**
     * This method is a wrapper for the ini_set() function that automatically
     * resets the modified php.ini setting to its original value after the
     * test is run.
     *
     * @param string $varName
     * @param string $newValue
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 3.0.0
     */
    protected function iniSet($varName, $newValue)
    {
        if (!is_string($varName)) {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'string');
        }

        $currentValue = ini_set($varName, $newValue);

        if ($currentValue !== false) {
            $this->iniSettings[$varName] = $currentValue;
        } else {
            throw new PHPUnit_Framework_Exception(
                sprintf(
                    'INI setting "%s" could not be set to "%s".',
                    $varName,
                    $newValue
                )
            );
        }
    }

    /**
     * This method is a wrapper for the setlocale() function that automatically
     * resets the locale to its original value after the test is run.
     *
     * @param int    $category
     * @param string $locale
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 3.1.0
     */
    protected function setLocale()
    {
        $args = func_get_args();

        if (count($args) < 2) {
            throw new PHPUnit_Framework_Exception;
        }

        $category = $args[0];
        $locale   = $args[1];

        $categories = [
            LC_ALL, LC_COLLATE, LC_CTYPE, LC_MONETARY, LC_NUMERIC, LC_TIME
        ];

        if (defined('LC_MESSAGES')) {
            $categories[] = LC_MESSAGES;
        }

        if (!in_array($category, $categories)) {
            throw new PHPUnit_Framework_Exception;
        }

        if (!is_array($locale) && !is_string($locale)) {
            throw new PHPUnit_Framework_Exception;
        }

        $this->locale[$category] = setlocale($category, 0);

        $result = call_user_func_array('setlocale', $args);

        if ($result === false) {
            throw new PHPUnit_Framework_Exception(
                'The locale functionality is not implemented on your platform, ' .
                'the specified locale does not exist or the category name is ' .
                'invalid.'
            );
        }
    }

    /**
     * Returns a builder object to create mock objects using a fluent interface.
     *
     * @param string $className
     *
     * @return PHPUnit_Framework_MockObject_MockBuilder
     *
     * @since Method available since Release 3.5.0
     */
    public function getMockBuilder($className)
    {
      throw new Exception('getMockBuilder is deprecated in the hacklang version');
      //  return new PHPUnit_Framework_MockObject_MockBuilder($this, $className);
    }

    /**
     * Returns a test double for the specified class.
     *
     * @param string $originalClassName
     *
     * @return PHPUnit_Framework_MockObject_MockObject
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 5.4.0
     */
    protected function createMock($originalClassName)
    {
      throw new Exception('createMock is deprecated in the hacklang version');
        // return $this->getMockBuilder($originalClassName)
        //             ->disableOriginalConstructor()
        //             ->disableOriginalClone()
        //             ->disableArgumentCloning()
        //             ->disallowMockingUnknownTypes()
        //             ->getMock();
    }

    /**
     * Returns a mock object for the specified class.
     *
     * @param string     $originalClassName       Name of the class to mock.
     * @param array|null $methods                 When provided, only methods whose names are in the array
     *                                            are replaced with a configurable test double. The behavior
     *                                            of the other methods is not changed.
     *                                            Providing null means that no methods will be replaced.
     * @param array      $arguments               Parameters to pass to the original class' constructor.
     * @param string     $mockClassName           Class name for the generated test double class.
     * @param bool       $callOriginalConstructor Can be used to disable the call to the original class' constructor.
     * @param bool       $callOriginalClone       Can be used to disable the call to the original class' clone constructor.
     * @param bool       $callAutoload            Can be used to disable __autoload() during the generation of the test double class.
     * @param bool       $cloneArguments
     * @param bool       $callOriginalMethods
     * @param object     $proxyTarget
     *
     * @return PHPUnit_Framework_MockObject_MockObject
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 3.0.0
     * @deprecated Method deprecated since Release 5.4.0
     */
    protected function getMock($originalClassName, $methods = [], array $arguments = [], $mockClassName = '', $callOriginalConstructor = true, $callOriginalClone = true, $callAutoload = true, $cloneArguments = false, $callOriginalMethods = false, $proxyTarget = null)
    {
      throw new Exception('getMock is disabled within the hacklang version.');
        //
        // $this->warnings[] = 'PHPUnit_Framework_TestCase::getMock() is deprecated, use PHPUnit_Framework_TestCase::createMock() or PHPUnit_Framework_TestCase::getMockBuilder() instead';
        //
        // $mockObject = $this->getMockObjectGenerator()->getMock(
        //     $originalClassName,
        //     $methods,
        //     $arguments,
        //     $mockClassName,
        //     $callOriginalConstructor,
        //     $callOriginalClone,
        //     $callAutoload,
        //     $cloneArguments,
        //     $callOriginalMethods,
        //     $proxyTarget
        // );
        //
        // $this->registerMockObject($mockObject);
        //
        // return $mockObject;
    }

    /**
     * Returns a mock with disabled constructor object for the specified class.
     *
     * @param string $originalClassName
     *
     * @return PHPUnit_Framework_MockObject_MockObject
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 5.0.0
     * @deprecated Method deprecated since Release 5.4.0
     */
    protected function getMockWithoutInvokingTheOriginalConstructor($originalClassName)
    {
      throw new Exception('getMockWithoutInvokingTheOriginalConstructor is disabled in the hacklang version');
        // $this->warnings[] = 'PHPUnit_Framework_TestCase::getMockWithoutInvokingTheOriginalConstructor() is deprecated, use PHPUnit_Framework_TestCase::createMock() instead';
        //
        // return $this->getMockBuilder($originalClassName)
        //             ->disableOriginalConstructor()
        //             ->getMock();
    }

    /**
     * Mocks the specified class and returns the name of the mocked class.
     *
     * @param string $originalClassName
     * @param array  $methods
     * @param array  $arguments
     * @param string $mockClassName
     * @param bool   $callOriginalConstructor
     * @param bool   $callOriginalClone
     * @param bool   $callAutoload
     * @param bool   $cloneArguments
     *
     * @return string
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 3.5.0
     */
    protected function getMockClass($originalClassName, $methods = [], array $arguments = [], $mockClassName = '', $callOriginalConstructor = false, $callOriginalClone = true, $callAutoload = true, $cloneArguments = false)
    {
      throw new Exception('getMockClass is disabled within the hacklang version');
        // $mock = $this->getMockObjectGenerator()->getMock(
        //     $originalClassName,
        //     $methods,
        //     $arguments,
        //     $mockClassName,
        //     $callOriginalConstructor,
        //     $callOriginalClone,
        //     $callAutoload,
        //     $cloneArguments
        // );
        //
        // return get_class($mock);
    }

    /**
     * Returns a mock object for the specified abstract class with all abstract
     * methods of the class mocked. Concrete methods are not mocked by default.
     * To mock concrete methods, use the 7th parameter ($mockedMethods).
     *
     * @param string $originalClassName
     * @param array  $arguments
     * @param string $mockClassName
     * @param bool   $callOriginalConstructor
     * @param bool   $callOriginalClone
     * @param bool   $callAutoload
     * @param array  $mockedMethods
     * @param bool   $cloneArguments
     *
     * @return PHPUnit_Framework_MockObject_MockObject
     *
     * @since Method available since Release 3.4.0
     *
     * @throws PHPUnit_Framework_Exception
     */
    protected function getMockForAbstractClass($originalClassName, array $arguments = [], $mockClassName = '', $callOriginalConstructor = true, $callOriginalClone = true, $callAutoload = true, $mockedMethods = [], $cloneArguments = false)
    {
      throw new Exception('getMockForAbstractClass is disabled within the hacklang version');
        // $mockObject = $this->getMockObjectGenerator()->getMockForAbstractClass(
        //     $originalClassName,
        //     $arguments,
        //     $mockClassName,
        //     $callOriginalConstructor,
        //     $callOriginalClone,
        //     $callAutoload,
        //     $mockedMethods,
        //     $cloneArguments
        // );
        //
        // $this->registerMockObject($mockObject);
        //
        // return $mockObject;
    }

    /**
     * Returns a mock object based on the given WSDL file.
     *
     * @param string $wsdlFile
     * @param string $originalClassName
     * @param string $mockClassName
     * @param array  $methods
     * @param bool   $callOriginalConstructor
     * @param array  $options                 An array of options passed to SOAPClient::_construct
     *
     * @return PHPUnit_Framework_MockObject_MockObject
     *
     * @since Method available since Release 3.4.0
     */
    protected function getMockFromWsdl($wsdlFile, $originalClassName = '', $mockClassName = '', array $methods = [], $callOriginalConstructor = true, array $options = [])
    {
      throw new Exception('getMockFromWsdl is disabled within the hacklang version');
        // if ($originalClassName === '') {
        //     $originalClassName = str_replace('.wsdl', '', basename($wsdlFile));
        // }
        //
        // if (!class_exists($originalClassName)) {
        //     eval(
        //         $this->getMockObjectGenerator()->generateClassFromWsdl(
        //             $wsdlFile,
        //             $originalClassName,
        //             $methods,
        //             $options
        //         )
        //     );
        // }
        //
        // $mockObject = $this->getMockObjectGenerator()->getMock(
        //     $originalClassName,
        //     $methods,
        //     ['', $options],
        //     $mockClassName,
        //     $callOriginalConstructor,
        //     false,
        //     false
        // );
        //
        // $this->registerMockObject($mockObject);
        //
        // return $mockObject;
    }

    /**
     * Returns a mock object for the specified trait with all abstract methods
     * of the trait mocked. Concrete methods to mock can be specified with the
     * `$mockedMethods` parameter.
     *
     * @param string $traitName
     * @param array  $arguments
     * @param string $mockClassName
     * @param bool   $callOriginalConstructor
     * @param bool   $callOriginalClone
     * @param bool   $callAutoload
     * @param array  $mockedMethods
     * @param bool   $cloneArguments
     *
     * @return PHPUnit_Framework_MockObject_MockObject
     *
     * @since Method available since Release 4.0.0
     *
     * @throws PHPUnit_Framework_Exception
     */
    protected function getMockForTrait($traitName, array $arguments = [], $mockClassName = '', $callOriginalConstructor = true, $callOriginalClone = true, $callAutoload = true, $mockedMethods = [], $cloneArguments = false)
    {
      throw new Exception('getMockForTrait is disabled in the hacklang version.');
        // $mockObject = $this->getMockObjectGenerator()->getMockForTrait(
        //     $traitName,
        //     $arguments,
        //     $mockClassName,
        //     $callOriginalConstructor,
        //     $callOriginalClone,
        //     $callAutoload,
        //     $mockedMethods,
        //     $cloneArguments
        // );
        //
        // $this->registerMockObject($mockObject);
        //
        // return $mockObject;
    }

    /**
     * Returns an object for the specified trait.
     *
     * @param string $traitName
     * @param array  $arguments
     * @param string $traitClassName
     * @param bool   $callOriginalConstructor
     * @param bool   $callOriginalClone
     * @param bool   $callAutoload
     * @param bool   $cloneArguments
     *
     * @return object
     *
     * @since Method available since Release 3.6.0
     *
     * @throws PHPUnit_Framework_Exception
     */
    protected function getObjectForTrait($traitName, array $arguments = [], $traitClassName = '', $callOriginalConstructor = true, $callOriginalClone = true, $callAutoload = true, $cloneArguments = false)
    {
        return $this->getMockObjectGenerator()->getObjectForTrait(
            $traitName,
            $arguments,
            $traitClassName,
            $callOriginalConstructor,
            $callOriginalClone,
            $callAutoload,
            $cloneArguments
        );
    }

    /**
     * @param string|null $classOrInterface
     *
     * @return \Prophecy\Prophecy\ObjectProphecy
     *
     * @throws \LogicException
     *
     * @since Method available since Release 4.5.0
     */
    protected function prophesize($classOrInterface = null)
    {
        return $this->getProphet()->prophesize($classOrInterface);
    }

    /**
     * Adds a value to the assertion counter.
     *
     * @param int $count
     *
     * @since Method available since Release 3.3.3
     */
    public function addToAssertionCount($count)
    {
        $this->numAssertions += $count;
    }

    /**
     * Returns the number of assertions performed by this test.
     *
     * @return int
     *
     * @since Method available since Release 3.3.0
     */
    public function getNumAssertions()
    {
        return $this->numAssertions;
    }

    /**
     * Returns a matcher that matches when the method is executed
     * zero or more times.
     *
     * @return PHPUnit_Framework_MockObject_Matcher_AnyInvokedCount
     *
     * @since Method available since Release 3.0.0
     */
    public static function any()
    {
        return new PHPUnit_Framework_MockObject_Matcher_AnyInvokedCount;
    }

    /**
     * Returns a matcher that matches when the method is never executed.
     *
     * @return PHPUnit_Framework_MockObject_Matcher_InvokedCount
     *
     * @since Method available since Release 3.0.0
     */
    public static function never()
    {
        return new PHPUnit_Framework_MockObject_Matcher_InvokedCount(0);
    }

    /**
     * Returns a matcher that matches when the method is executed
     * at least N times.
     *
     * @param int $requiredInvocations
     *
     * @return PHPUnit_Framework_MockObject_Matcher_InvokedAtLeastCount
     *
     * @since Method available since Release 4.2.0
     */
    public static function atLeast($requiredInvocations)
    {
        return new PHPUnit_Framework_MockObject_Matcher_InvokedAtLeastCount(
            $requiredInvocations
        );
    }

    /**
     * Returns a matcher that matches when the method is executed at least once.
     *
     * @return PHPUnit_Framework_MockObject_Matcher_InvokedAtLeastOnce
     *
     * @since Method available since Release 3.0.0
     */
    public static function atLeastOnce()
    {
        return new PHPUnit_Framework_MockObject_Matcher_InvokedAtLeastOnce;
    }

    /**
     * Returns a matcher that matches when the method is executed exactly once.
     *
     * @return PHPUnit_Framework_MockObject_Matcher_InvokedCount
     *
     * @since Method available since Release 3.0.0
     */
    public static function once()
    {
        return new PHPUnit_Framework_MockObject_Matcher_InvokedCount(1);
    }

    /**
     * Returns a matcher that matches when the method is executed
     * exactly $count times.
     *
     * @param int $count
     *
     * @return PHPUnit_Framework_MockObject_Matcher_InvokedCount
     *
     * @since Method available since Release 3.0.0
     */
    public static function exactly($count)
    {
        return new PHPUnit_Framework_MockObject_Matcher_InvokedCount($count);
    }

    /**
     * Returns a matcher that matches when the method is executed
     * at most N times.
     *
     * @param int $allowedInvocations
     *
     * @return PHPUnit_Framework_MockObject_Matcher_InvokedAtMostCount
     *
     * @since Method available since Release 4.2.0
     */
    public static function atMost($allowedInvocations)
    {
        return new PHPUnit_Framework_MockObject_Matcher_InvokedAtMostCount(
            $allowedInvocations
        );
    }

    /**
     * Returns a matcher that matches when the method is executed
     * at the given index.
     *
     * @param int $index
     *
     * @return PHPUnit_Framework_MockObject_Matcher_InvokedAtIndex
     *
     * @since Method available since Release 3.0.0
     */
    public static function at($index)
    {
        return new PHPUnit_Framework_MockObject_Matcher_InvokedAtIndex($index);
    }

    /**
     * @param mixed $value
     *
     * @return PHPUnit_Framework_MockObject_Stub_Return
     *
     * @since Method available since Release 3.0.0
     */
    public static function returnValue($value)
    {
        return new PHPUnit_Framework_MockObject_Stub_Return($value);
    }

    /**
     * @param array $valueMap
     *
     * @return PHPUnit_Framework_MockObject_Stub_ReturnValueMap
     *
     * @since Method available since Release 3.6.0
     */
    public static function returnValueMap(array $valueMap)
    {
        return new PHPUnit_Framework_MockObject_Stub_ReturnValueMap($valueMap);
    }

    /**
     * @param int $argumentIndex
     *
     * @return PHPUnit_Framework_MockObject_Stub_ReturnArgument
     *
     * @since Method available since Release 3.3.0
     */
    public static function returnArgument($argumentIndex)
    {
        return new PHPUnit_Framework_MockObject_Stub_ReturnArgument(
            $argumentIndex
        );
    }

    /**
     * @param mixed $callback
     *
     * @return PHPUnit_Framework_MockObject_Stub_ReturnCallback
     *
     * @since Method available since Release 3.3.0
     */
    public static function returnCallback($callback)
    {
        return new PHPUnit_Framework_MockObject_Stub_ReturnCallback($callback);
    }

    /**
     * Returns the current object.
     *
     * This method is useful when mocking a fluent interface.
     *
     * @return PHPUnit_Framework_MockObject_Stub_ReturnSelf
     *
     * @since Method available since Release 3.6.0
     */
    public static function returnSelf()
    {
        return new PHPUnit_Framework_MockObject_Stub_ReturnSelf();
    }

    /**
     * @param Throwable|Exception $exception
     *
     * @return PHPUnit_Framework_MockObject_Stub_Exception
     *
     * @since Method available since Release 3.1.0
     *
     * @todo   Add type declaration when support for PHP 5 is dropped
     */
    public static function throwException($exception)
    {
        return new PHPUnit_Framework_MockObject_Stub_Exception($exception);
    }

    /**
     * @param mixed $value, ...
     *
     * @return PHPUnit_Framework_MockObject_Stub_ConsecutiveCalls
     *
     * @since Method available since Release 3.0.0
     */
    public static function onConsecutiveCalls()
    {
        $args = func_get_args();

        return new PHPUnit_Framework_MockObject_Stub_ConsecutiveCalls($args);
    }

    /**
     * @return bool
     *
     * @since Method available since Release 5.4.0
     */
    public function usesDataProvider()
    {
        return !empty($this->data);
    }

    /**
     * @return string
     *
     * @since Method available since Release 5.4.0
     */
    public function dataDescription()
    {
        return is_string($this->dataName) ? $this->dataName : '';
    }

    /**
     * Gets the data set description of a TestCase.
     *
     * @param bool $includeData
     *
     * @return string
     *
     * @since Method available since Release 3.3.0
     */
    protected function getDataSetAsString($includeData = true)
    {
        $buffer = '';

        if (!empty($this->data)) {
            if (is_int($this->dataName)) {
                $buffer .= sprintf(' with data set #%d', $this->dataName);
            } else {
                $buffer .= sprintf(' with data set "%s"', $this->dataName);
            }

            $exporter = new Exporter;

            if ($includeData) {
                $buffer .= sprintf(' (%s)', $exporter->shortenedRecursiveExport($this->data));
            }
        }

        return $buffer;
    }

    /**
     * Creates a default TestResult object.
     *
     * @return PHPUnit_Framework_TestResult
     */
    protected function createResult()
    {
        return new PHPUnit_Framework_TestResult;
    }

    /**
     * @since Method available since Release 3.5.4
     */
    protected function handleDependencies()
    {
      $test = $this->getTest();

        if (!empty($this->dependencies) && !$this->inIsolation) {
            $className  = get_class($test);
            //$passed     = $this->result->passed();
            $passed = $test->getTestResultObject()->passed();

            $passedKeys = array_keys($passed);
            $numKeys    = count($passedKeys);

            for ($i = 0; $i < $numKeys; $i++) {
                $pos = strpos($passedKeys[$i], ' with data set');

                if ($pos !== false) {
                    $passedKeys[$i] = substr($passedKeys[$i], 0, $pos);
                }
            }

            $passedKeys = array_flip(array_unique($passedKeys));

            foreach ($this->dependencies as $dependency) {
                $clone = false;

                if (strpos($dependency, 'clone ') === 0) {
                    $clone      = true;
                    $dependency = substr($dependency, strlen('clone '));
                } elseif (strpos($dependency, '!clone ') === 0) {
                    $clone      = false;
                    $dependency = substr($dependency, strlen('!clone '));
                }

                if (strpos($dependency, '::') === false) {
                    $dependency = $className . '::' . $dependency;
                }

                if (!isset($passedKeys[$dependency])) {
                    $this->result->addError(
                        $this,
                        new PHPUnit_Framework_SkippedTestError(
                            sprintf(
                                'This test depends on "%s" to pass.',
                                $dependency
                            )
                        ),
                        0
                    );

                    return false;
                }

                if (isset($passed[$dependency])) {
                    if ($passed[$dependency]['size'] != PHPUnit_Util_Test::UNKNOWN &&
                        $this->getSize() != PHPUnit_Util_Test::UNKNOWN &&
                        $passed[$dependency]['size'] > $this->getSize()) {
                        $this->result->addError(
                            $this,
                            new PHPUnit_Framework_SkippedTestError(
                                'This test depends on a test that is larger than itself.'
                            ),
                            0
                        );

                        return false;
                    }

                    if ($clone) {
                        $deepCopy   = new DeepCopy;
                        $deepCopy->skipUncloneable(false);

                        $this->dependencyInput[$dependency] = $deepCopy->copy($passed[$dependency]['result']);
                    } else {
                        $this->dependencyInput[$dependency] = $passed[$dependency]['result'];
                    }
                } else {
                    $this->dependencyInput[$dependency] = null;
                }
            }
        }

        return true;
    }

    /**
     * This method is called before the first test of this test class is run.
     *
     * @since Method available since Release 3.4.0
     */
    public static function setUpBeforeClass()
    {
    }

    /**
     * Sets up the fixture, for example, open a network connection.
     * This method is called before a test is executed.
     */
    protected function setUp()
    {
    }

    /**
     * Performs assertions shared by all tests of a test case.
     *
     * This method is called before the execution of a test starts
     * and after setUp() is called.
     *
     * @since Method available since Release 3.2.8
     */
    protected function assertPreConditions()
    {
    }

    /**
     * Performs assertions shared by all tests of a test case.
     *
     * This method is called before the execution of a test ends
     * and before tearDown() is called.
     *
     * @since Method available since Release 3.2.8
     */
    protected function assertPostConditions()
    {
    }

    /**
     * Tears down the fixture, for example, close a network connection.
     * This method is called after a test is executed.
     */
    protected function tearDown()
    {
    }

    /**
     * This method is called after the last test of this test class is run.
     *
     * @since Method available since Release 3.4.0
     */
    public static function tearDownAfterClass()
    {
    }

    /**
     * This method is called when a test method did not execute successfully.
     *
     * @param Exception|Throwable $e
     *
     * @since Method available since Release 3.4.0
     *
     * @throws Exception|Throwable
     */
    protected function onNotSuccessfulTest($e)
    {
        $expected = PHP_MAJOR_VERSION >= 7 ? 'Throwable' : 'Exception';

        if ($e instanceof $expected) {
            throw $e;
        }

        throw PHPUnit_Util_InvalidArgumentHelper::factory(
            1,
            'Throwable or Exception'
        );
    }

    /**
     * Performs custom preparations on the process isolation template.
     *
     * @param Text_Template $template
     *
     * @since Method available since Release 3.4.0
     */
    protected function prepareTemplate(Text_Template $template)
    {
    }

    /**
     * Get the mock object generator, creating it if it doesn't exist.
     *
     * @return PHPUnit_Framework_MockObject_Generator
     */
    protected function getMockObjectGenerator()
    {
        if (null === $this->mockObjectGenerator) {
            $this->mockObjectGenerator = new PHPUnit_Framework_MockObject_Generator;
        }

        return $this->mockObjectGenerator;
    }

    /**
     * @since Method available since Release 4.2.0
     */
    private function startOutputBuffering() {

      OutputBuffering::start();

      $this->outputBufferingLevel = OutputBuffering::getCurrentLevel();

    }

    /**
     * @since Method available since Release 4.2.0
     */
    private function stopOutputBuffering()
    {

      if ( OutputBuffering::getCurrentLevel() != $this->outputBufferingLevel ) {
        throw new PHPUnit_Framework_RiskyTestError(
                 'Test code or tested code did not (only) close its own output buffers'
        );
      }

        $output = OutputBuffering::get();

        if ($this->outputCallback === false) {
            $this->output = $output;
        } else {
            $this->output = call_user_func_array(
                $this->outputCallback,
                [$output]
            );
        }

        OutputBuffering::end();

    }

    private function snapshotGlobalState() {
      // --
      // JEO:
      // Snapshoting global state is a massive performance penalty as it snapshots
      // all of the state of the coverage environment too, we removed this.
      // --
      //  $backupGlobals = $this->backupGlobals === null || $this->backupGlobals === true;
      //
      //  if ($this->runTestInSeparateProcess || $this->inIsolation ||
      //      (!$backupGlobals && !$this->backupStaticAttributes)) {
      //      return;
      //  }
      //
      //  $this->snapshot = $this->createGlobalStateSnapshot($backupGlobals);
    }

    private function restoreGlobalState()
    {
      // --
      // JEO:
      // Snapshoting global state is a massive performance penalty as it snapshots
      // all of the state of the coverage environment too, we removed this.
      // --
      //  if (!$this->snapshot instanceof Snapshot) {
      //      return;
      //  }
      //
      //  $backupGlobals = $this->backupGlobals === null || $this->backupGlobals === true;
      //
      //  if ($this->beStrictAboutChangesToGlobalState) {
      //      try {
      //          $this->compareGlobalStateSnapshots(
      //              $this->snapshot,
      //              $this->createGlobalStateSnapshot($backupGlobals)
      //          );
      //      } catch (PHPUnit_Framework_RiskyTestError $rte) {
      //          // Intentionally left empty
      //      }
      //  }
      //
      //  $restorer = new Restorer;
      //
      //  if ($backupGlobals) {
      //      $restorer->restoreGlobalVariables($this->snapshot);
      //  }
      //
      //  if ($this->backupStaticAttributes) {
      //      $restorer->restoreStaticAttributes($this->snapshot);
      //  }
      //
      //  $this->snapshot = null;
      //
      //  if (isset($rte)) {
      //      throw $rte;
      //  }
    }

    /**
     * @param bool $backupGlobals
     *
     * @return Snapshot
     */
    private function createGlobalStateSnapshot($backupGlobals)
    {
      // --
      // JEO:
      // Snapshoting global state is a massive performance penalty as it snapshots
      // all of the state of the coverage environment too, we removed this.
      // --
      //  $blacklist = new Blacklist;
      //
      //  foreach ($this->backupGlobalsBlacklist as $globalVariable) {
      //      $blacklist->addGlobalVariable($globalVariable);
      //  }
      //
      //  if (!defined('PHPUNIT_TESTSUITE')) {
      //      $blacklist->addClassNamePrefix('PHPUnit');
      //      $blacklist->addClassNamePrefix('File_Iterator');
      //      $blacklist->addClassNamePrefix('SebastianBergmann\CodeCoverage');
      //      $blacklist->addClassNamePrefix('PHP_Invoker');
      //      $blacklist->addClassNamePrefix('PHP_Timer');
      //      $blacklist->addClassNamePrefix('PHP_Token');
      //      $blacklist->addClassNamePrefix('Symfony');
      //      $blacklist->addClassNamePrefix('Text_Template');
      //      $blacklist->addClassNamePrefix('Doctrine\Instantiator');
      //
      //      foreach ($this->backupStaticAttributesBlacklist as $class => $attributes) {
      //          foreach ($attributes as $attribute) {
      //              $blacklist->addStaticAttribute($class, $attribute);
      //          }
      //      }
      //  }
      //
      //  return new Snapshot(
      //      $blacklist,
      //      $backupGlobals,
      //      $this->backupStaticAttributes,
      //      false,
      //      false,
      //      false,
      //      false,
      //      false,
      //      false,
      //      false
      //  );
    }

    /**
     * @param Snapshot $before
     * @param Snapshot $after
     *
     * @throws PHPUnit_Framework_RiskyTestError
     */
    private function compareGlobalStateSnapshots(Snapshot $before, Snapshot $after)
    {
      // JEO: Disabled.
      /*
        $backupGlobals = $this->backupGlobals === null || $this->backupGlobals === true;

        if ($backupGlobals) {
            $this->compareGlobalStateSnapshotPart(
                $before->globalVariables(),
                $after->globalVariables(),
                "--- Global variables before the test\n+++ Global variables after the test\n"
            );

            $this->compareGlobalStateSnapshotPart(
                $before->superGlobalVariables(),
                $after->superGlobalVariables(),
                "--- Super-global variables before the test\n+++ Super-global variables after the test\n"
            );
        }

        if ($this->backupStaticAttributes) {
            $this->compareGlobalStateSnapshotPart(
                $before->staticAttributes(),
                $after->staticAttributes(),
                "--- Static attributes before the test\n+++ Static attributes after the test\n"
            );
        }
      */
    }

    /**
     * @param array  $before
     * @param array  $after
     * @param string $header
     *
     * @throws PHPUnit_Framework_RiskyTestError
     */
    private function compareGlobalStateSnapshotPart(array $before, array $after, $header)
    {
      // JEO: Disabled.
      /*
        if ($before != $after) {
            $differ   = new Differ($header);
            $exporter = new Exporter;

            $diff = $differ->diff(
                $exporter->export($before),
                $exporter->export($after)
            );

            throw new PHPUnit_Framework_RiskyTestError(
                $diff
            );
        }
       */
    }

    /**
     * @return Prophecy\Prophet
     *
     * @since Method available since Release 4.5.0
     */
    private function getProphet()
    {
        if ($this->prophet === null) {
            $this->prophet = new Prophet;
        }

        return $this->prophet;
    }

    /**
     * @param PHPUnit_Framework_MockObject_MockObject $mock
     *
     * @return bool
     *
     * @since Method available since Release 5.0.0
     */
    private function shouldInvocationMockerBeReset(PHPUnit_Framework_MockObject_MockObject $mock)
    {
        $enumerator = new Enumerator;

        foreach ($enumerator->enumerate($this->dependencyInput) as $object) {
            if ($mock === $object) {
                return false;
            }
        }

        if (!is_array($this->testResult) && !is_object($this->testResult)) {
            return true;
        }

        foreach ($enumerator->enumerate($this->testResult) as $object) {
            if ($mock === $object) {
                return false;
            }
        }

        return true;
    }

    /**
     * @param array $testArguments
     *
     * @since Method available since Release 5.0.0
     */
    private function registerMockObjectsFromTestArguments(array $testArguments)
    {
        if ($this->registerMockObjectsFromTestArgumentsRecursively) {
            $enumerator = new Enumerator;

            foreach ($enumerator->enumerate($testArguments) as $object) {
                if ($object instanceof PHPUnit_Framework_MockObject_MockObject) {
                    $this->registerMockObject($object);
                }
            }
        } else {
            foreach ($testArguments as $testArgument) {
                if ($testArgument instanceof PHPUnit_Framework_MockObject_MockObject) {
                    $this->registerMockObject($testArgument);
                } elseif (is_array($testArgument)) {
                    $this->registerMockObjectsFromTestArguments($testArgument);
                }
            }
        }
    }
}
