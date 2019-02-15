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
     * @var SebastianBergmann\GlobalState\Snapshot
     */
    private $snapshot;

    /**
     * @var bool
     */
    private $beStrictAboutChangesToGlobalState = false;

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
      parent::__construct(strval($name));

        $this->data     = $data;
        $this->dataName = $dataName;

    }


    // @TODO: Save for later as we will need to be almost completely ported.
    public function setTest(PHPUnit_Framework_Test $test) {
      $this->_test = $test;
      return true;
    }

    // @TODO: Save for later as we will need to be almost completely ported.
    public function getTest(): PHPUnit_Framework_Test {
      if ( $this->_test === null ) {
        return $this;
      }
      return $this->_test;
    }

    /**
     * Returns a string representation of the test case.
     *
     * @return string
     */
    public function toString() {

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
     * @return bool
     *
     * @since Method available since Release 3.6.0
     */
    public function hasOutput()
    {
        if (strlen($this->getActualOutput()) === 0) {
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
            throw new PHPUnit_Framework_Exception();
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
            throw new PHPUnit_Framework_Exception();
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
              $output = $this->getActualOutput();

              if ($this->outputExpectedRegex !== null) {
                $this->assertRegExp($this->outputExpectedRegex, $output);
              } elseif ($this->outputExpectedString !== null) {
                $this->assertEquals($this->outputExpectedString, $output);
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
            throw new PHPUnit_Framework_Exception();
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
            throw new PHPUnit_Framework_Exception();
        }

        if (!is_array($locale) && !is_string($locale)) {
            throw new PHPUnit_Framework_Exception();
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

            $exporter = new Exporter();

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
        return new PHPUnit_Framework_TestResult();
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
                        $deepCopy   = new DeepCopy();
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

}
