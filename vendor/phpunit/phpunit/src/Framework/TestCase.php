<?php

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use SebastianBergmann\Exporter\Exporter;
use SebastianBergmann\PHPUnit\Environment\OutputBuffering;
use SebastianBergmann\PHPUnit\Exceptions\AssertionFailedException;
use SebastianBergmann\PHPUnit\Exceptions\WarningException;
use SebastianBergmann\PHPUnit\Exceptions\TestError\IncompleteException;
use SebastianBergmann\PHPUnit\Exceptions\TestError\SkippedException;
use Prophecy\Exception\Prediction\PredictionException;
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
     * @var array
     */
    private $data = [];

    /**
     * @var string
     */
    private $dataName = '';

    /**
     * @var array
     */
    private $dependencies = [];

    /**
     * @var array
     */
    private $dependencyInput = [];

    /**
     * @var PHPUnit_Framework_TestResult
     */
    private $result;

    /**
     * @var mixed
     */
    private $testResult;

    /**
     * @var string[]
     */
    private $warnings = [];

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

      $buffer = sprintf(
        '%s::%s',
        $this->getClass(),
        $this->getName(false)
      );

      return $buffer . $this->getDataSetAsString();

    }

    /**
     * @since Method available since Release 3.6.0
     */
    protected function checkRequirements()
    {
      $test = $this->getTest();
      $name = $this->getName(false);
        if (! $name || !method_exists($test, $name)) {
            return;
        }

        $missingRequirements = PHPUnit_Util_Test::getMissingRequirements(
            $test->getClass(),
            $name
        );

        if (count($missingRequirements) > 0) {
            $this->markTestSkipped(implode(PHP_EOL, $missingRequirements));
        }
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

        // var_dump('run=' . get_class($this) . ' test=' . get_class($test));


        if ($result === null) {
            $result = $this->createResult();
        }

        if (!$this instanceof PHPUnit_Framework_WarningTestCase) {
            $this->setTestResultObject($result);
            $this->setUseErrorHandlerFromAnnotation($test);
        }

        $oldErrorHandlerSetting = null;

        if ($this->getUseErrorHandler() !== null) {
            $oldErrorHandlerSetting = $result->getConvertErrorsToExceptions();
            $result->convertErrorsToExceptions($this->getUseErrorHandler());
        }

        if (!$this instanceof PHPUnit_Framework_WarningTestCase && !$this->handleDependencies()) {
            return;
        }

        // Pass on the test into the run function for result.
        $result->run($test);

        if (is_bool($oldErrorHandlerSetting)) {
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

      // var_dump('runBare=' . get_class($this) . ' test=' . get_class($test));

        $this->startOutputBuffering();

        clearstatcache();

        $currentWorkingDirectory = getcwd();

        $hookMethods = PHPUnit_Util_Test::getHookMethods($test->getClass());

        try {
            $hasMetRequirements = false;
            $this->checkRequirements();
            $hasMetRequirements = true;

            $test->setExpectedExceptionFromAnnotation();

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
                throw new WarningException(
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
        } catch (SkippedException $e) {
          $this->status()->setMessageAndCode(
            $e->getMessage(),
            PHPUnit_Runner_BaseTestRunner::STATUS_SKIPPED
          );
        } catch (WarningException $e) {
          $this->status()->setMessageAndCode(
            $e->getMessage(),
            PHPUnit_Runner_BaseTestRunner::STATUS_WARNING
          );
        } catch (AssertionFailedException $e) {
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
        $this->clearIniSettings();

        // Clean up locale settings.
        $this->clearLocales();

        // Perform assertion on output.
        if (!isset($e)) {

            try {
              $output = $this->getActualOutput();

              $expectedOutputRegex = $this->getExpectedOutputRegex();
              $expectedOutput = $this->getExpectedOutput();

              if (is_string($expectedOutputRegex)) {
                $this->assertRegExp($expectedOutputRegex, $output);
              } elseif (is_string($expectedOutput)) {
                $this->assertEquals($expectedOutput, $output);
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
                $e = new AssertionFailedException($e->getMessage());
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

        if ( $this->getExpectedException() != '' ) {
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


            $this->assertExceptionSameClass($this->getExpectedException(), $e);

            $expectedExceptionMessage = $this->getExpectedExceptionMessage();
            $expectedExceptionMessageRegExp = $this->getExpectedExceptionMessageRegExp();

            if ($expectedExceptionMessage != '') {
              $this->assertExceptionSameMessage($expectedExceptionMessage, $e);
            }

            if ($expectedExceptionMessageRegExp != '') {
              $this->assertExceptionRegexpMessage($expectedExceptionMessageRegExp, $e);
            }

            if ($this->getExpectedExceptionCode() !== -1) {
              $this->assertEquals(
                $this->getExpectedExceptionCode(),
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

        if (!empty($this->dependencies)) {
            $className  = $test->getClass();
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
                        new SkippedException(
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
                            new SkippedException(
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

}
