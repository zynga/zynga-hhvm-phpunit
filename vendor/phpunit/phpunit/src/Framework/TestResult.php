<?php
/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use SebastianBergmann\CodeCoverage\CodeCoverage;
use SebastianBergmann\CodeCoverage\Exception as CodeCoverageException;
use SebastianBergmann\CodeCoverage\CoveredCodeNotExecutedException;
use SebastianBergmann\CodeCoverage\MissingCoversAnnotationException;
use SebastianBergmann\CodeCoverage\UnintentionallyCoveredCodeException;
use SebastianBergmann\ResourceOperations\ResourceOperations;
use Zynga\Framework\Testing\TestCase\V2\Base as ZyngaTestCaseBase;
use Zynga\Framework\ReflectionCache\V1\ReflectionClasses;

use Zynga\PHPUnit\V2\Interfaces\TestListenerInterface;
use Zynga\PHPUnit\V2\TestResult;

use SebastianBergmann\PHPUnit\Exceptions\AssertionFailedException;
use SebastianBergmann\PHPUnit\Exceptions\ExpectationFailedException;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentException;
use SebastianBergmann\PHPUnit\Exceptions\WarningException;
use SebastianBergmann\PHPUnit\Exceptions\TestError\IncompleteException;
use SebastianBergmann\PHPUnit\Exceptions\TestError\RiskyException;
use SebastianBergmann\PHPUnit\Exceptions\TestError\SkippedException;

/**
 * A TestResult collects the results of executing a test case.
 *
 * @since Class available since Release 2.0.0
 */
class PHPUnit_Framework_TestResult extends TestResult implements Countable
{
    /**
     * @var array
     */
    protected $passed = [];

    /**
     * @var array
     */
    protected $errors = [];

    /**
     * @var array
     */
    protected $failures = [];

    /**
     * @var array
     */
    protected $warnings = [];

    /**
     * @var array
     */
    protected $notImplemented = [];

    /**
     * @var array
     */
    protected $risky = [];

    /**
     * @var array
     */
    protected $skipped = [];

    /**
     * @var int
     */
    protected $runTests = 0;

    /**
     * @var float
     */
    protected $time = 0;

    /**
     * @var PHPUnit_Framework_TestSuite
     */
    protected $topTestSuite = null;

    /**
     * Code Coverage information.
     *
     * @var CodeCoverage
     */
    protected $codeCoverage;

    /**
     * @var bool
     */
    protected $convertErrorsToExceptions = true;

    /**
     * @var bool
     */
    protected $stop = false;

    /**
     * @var bool
     */
    protected $stopOnError = false;

    /**
     * @var bool
     */
    protected $stopOnFailure = false;

    /**
     * @var bool
     */
    protected $stopOnWarning = false;

    /**
     * @var bool
     */
    protected $beStrictAboutTestsThatDoNotTestAnything = false;

    /**
     * @var bool
     */
    protected $beStrictAboutOutputDuringTests = false;

    /**
     * @var bool
     */
    protected $beStrictAboutTodoAnnotatedTests = false;

    /**
     * @var bool
     */
    protected $beStrictAboutResourceUsageDuringSmallTests = false;

    /**
     * @var bool
     */
    protected $enforceTimeLimit = false;

    /**
     * @var int
     */
    protected $timeoutForSmallTests = 1;

    /**
     * @var int
     */
    protected $timeoutForMediumTests = 10;

    /**
     * @var int
     */
    protected $timeoutForLargeTests = 60;

    /**
     * @var bool
     */
    protected $stopOnRisky = false;

    /**
     * @var bool
     */
    protected $stopOnIncomplete = false;

    /**
     * @var bool
     */
    protected $stopOnSkipped = false;

    /**
     * @var bool
     */
    protected $lastTestFailed = false;

    /**
     * Adds an error to the list of errors.
     *
     * @param PHPUnit_Framework_Test $test
     * @param Throwable              $t
     * @param float                  $time
     */
    public function addError(PHPUnit_Framework_Test $test, $t, $time)
    {
        if ($t instanceof PHPUnit_Framework_RiskyTest) {

            $this->risky[] = new PHPUnit_Framework_TestFailure($test, $t);

            $this->listeners()->addRiskyTest($test, $t, $time);

            if ($this->stopOnRisky) {
                $this->stop();
            }

        } elseif ($t instanceof IncompleteException) {
            $this->notImplemented[] = new PHPUnit_Framework_TestFailure($test, $t);

            $this->listeners()->addIncompleteTest($test, $t, $time);

            if ($this->stopOnIncomplete) {
                $this->stop();
            }

        } else if ($t instanceof SkippedException ) {

          $this->skipped[] = new PHPUnit_Framework_TestFailure($test, $t);

          $this->listeners()->addSkippedTest($test, $t, $time);

          if ($this->stopOnSkipped) {
              $this->stop();
          }

        } else {
            $this->errors[] = new PHPUnit_Framework_TestFailure($test, $t);

            $this->listeners()->addError($test, $t, $time);

            if ($this->stopOnError || $this->stopOnFailure) {
                $this->stop();
            }
        }

        // @see https://github.com/sebastianbergmann/phpunit/issues/1953
        if ($t instanceof Error) {
            $t = new PHPUnit_Framework_ExceptionWrapper($t);
        }

        $this->lastTestFailed = true;
        $this->time          += $time;
    }

    /**
     * Adds a warning to the list of warnings.
     * The passed in exception caused the warning.
     *
     * @param PHPUnit_Framework_Test    $test
     * @param Exception $e
     * @param float                     $time
     *
     * @since Method available since Release 5.1.0
     */
    public function addWarning(PHPUnit_Framework_Test $test, Exception $e, $time)
    {
        if ($this->stopOnWarning) {
            $this->stop();
        }

        $this->warnings[] = new PHPUnit_Framework_TestFailure($test, $e);

        $this->listeners()->addWarning($test, $e, $time);

        $this->time += $time;
    }

    /**
     * Adds a failure to the list of failures.
     * The passed in exception caused the failure.
     *
     * @param PHPUnit_Framework_Test                 $test
     * @param Exception $e
     * @param float                                  $time
     */
    public function addFailure(PHPUnit_Framework_Test $test, Exception $e, $time)
    {
      // if ( preg_match('/Requirements/', get_class($test))) {
         // var_dump($test->getName());
         // var_dump(get_class($test));
         // var_dump($e);
         // exit();
      // }

        if ($e instanceof PHPUnit_Framework_RiskyTest ||
            $e instanceof PHPUnit_Framework_OutputError) {

            $this->risky[] = new PHPUnit_Framework_TestFailure($test, $e);
            $this->listeners()->addRiskyTest($test, $e, $time);

            if ($this->stopOnRisky) {
                $this->stop();
            }

        } elseif ($e instanceof IncompleteException) {

            $this->notImplemented[] = new PHPUnit_Framework_TestFailure($test, $e);

            $this->listeners()->addIncompleteTest($test, $e, $time);

            if ($this->stopOnIncomplete) {
                $this->stop();
            }

        } elseif ($e instanceof SkippedException) {

          $this->skipped[] = new PHPUnit_Framework_TestFailure($test, $e);

          $this->listeners()->addSkippedTest($test, $e, $time);

          if ($this->stopOnSkipped) {
              $this->stop();
          }

        } else {

            $this->failures[] = new PHPUnit_Framework_TestFailure($test, $e);

            $this->listeners()->addFailure($test, $e, $time);

            if ($this->stopOnFailure) {
                $this->stop();
            }

        }

        $this->lastTestFailed = true;
        $this->time          += $time;
    }

    /**
     * Informs the result that a testsuite will be started.
     *
     * @param PHPUnit_Framework_TestSuite $suite
     *
     * @since Method available since Release 2.2.0
     */
    public function startTestSuite(PHPUnit_Framework_TestSuite $suite)
    {
        if ($this->topTestSuite === null) {
            $this->topTestSuite = $suite;
        }

        $this->listeners()->startTestSuite($suite);
    }

    /**
     * Informs the result that a testsuite was completed.
     *
     * @param PHPUnit_Framework_TestSuite $suite
     *
     * @since Method available since Release 2.2.0
     */
    public function endTestSuite(PHPUnit_Framework_TestSuite $suite)
    {
      $this->listeners()->endTestSuite($suite);
    }

    /**
     * Informs the result that a test will be started.
     *
     * @param PHPUnit_Framework_Test $test
     */
    public function startTest(PHPUnit_Framework_Test $test)
    {
        $this->lastTestFailed = false;
        $this->runTests      += count($test);

        $this->listeners()->startTest($test);
    }

    /**
     * Informs the result that a test was completed.
     *
     * @param PHPUnit_Framework_Test $test
     * @param float                  $time
     */
    public function endTest(PHPUnit_Framework_Test $test, $time)
    {
      $this->listeners()->endTest($test, $time);

        if (!$this->lastTestFailed && ($test instanceof ZyngaTestCaseBase || $test instanceof \PHPUnit_Framework_TestCase)) {
            $class  = $test->getClass();
            $key    = $class . '::' . $test->getName();

            $this->passed[$key] = [
                'result' => $test->getResult(),
                'size'   => $test->getSize(
                    $class,
                    $test->getName(false)
                )
            ];

            $this->time += $time;
        }
    }

    /**
     * Returns true if no risky test occurred.
     *
     * @return bool
     *
     * @since Method available since Release 4.0.0
     */
    public function allHarmless()
    {
        return $this->riskyCount() == 0;
    }

    /**
     * Gets the number of risky tests.
     *
     * @return int
     *
     * @since Method available since Release 4.0.0
     */
    public function riskyCount()
    {
        return count($this->risky);
    }

    /**
     * Returns true if no incomplete test occurred.
     *
     * @return bool
     */
    public function allCompletelyImplemented()
    {
        return $this->notImplementedCount() == 0;
    }

    /**
     * Gets the number of incomplete tests.
     *
     * @return int
     */
    public function notImplementedCount()
    {
        return count($this->notImplemented);
    }

    /**
     * Returns an Enumeration for the risky tests.
     *
     * @return array
     *
     * @since Method available since Release 4.0.0
     */
    public function risky()
    {
        return $this->risky;
    }

    /**
     * Returns an Enumeration for the incomplete tests.
     *
     * @return array
     */
    public function notImplemented()
    {
        return $this->notImplemented;
    }

    /**
     * Returns true if no test has been skipped.
     *
     * @return bool
     *
     * @since Method available since Release 3.0.0
     */
    public function noneSkipped()
    {
        return $this->skippedCount() == 0;
    }

    /**
     * Gets the number of skipped tests.
     *
     * @return int
     *
     * @since Method available since Release 3.0.0
     */
    public function skippedCount()
    {
        return count($this->skipped);
    }

    /**
     * Returns an Enumeration for the skipped tests.
     *
     * @return array
     *
     * @since Method available since Release 3.0.0
     */
    public function skipped()
    {
        return $this->skipped;
    }

    /**
     * Gets the number of detected errors.
     *
     * @return int
     */
    public function errorCount()
    {
        return count($this->errors);
    }

    /**
     * Returns an Enumeration for the errors.
     *
     * @return array
     */
    public function errors()
    {
        return $this->errors;
    }

    /**
     * Gets the number of detected failures.
     *
     * @return int
     */
    public function failureCount()
    {
        return count($this->failures);
    }

    /**
     * Returns an Enumeration for the failures.
     *
     * @return array
     */
    public function failures()
    {
        return $this->failures;
    }

    /**
     * Gets the number of detected warnings.
     *
     * @return int
     *
     * @since Method available since Release 5.1.0
     */
    public function warningCount()
    {
        return count($this->warnings);
    }

    /**
     * Returns an Enumeration for the warnings.
     *
     * @return array
     *
     * @since Method available since Release 5.1.0
     */
    public function warnings()
    {
        return $this->warnings;
    }

    /**
     * Returns the names of the tests that have passed.
     *
     * @return array
     *
     * @since Method available since Release 3.4.0
     */
    public function passed()
    {
        return $this->passed;
    }

    /**
     * Returns the (top) test suite.
     *
     * @return PHPUnit_Framework_TestSuite
     *
     * @since Method available since Release 3.0.0
     */
    public function topTestSuite()
    {
        return $this->topTestSuite;
    }

    /**
     * Returns whether code coverage information should be collected.
     *
     * @return bool If code coverage should be collected
     *
     * @since Method available since Release 3.2.0
     */
    public function getCollectCodeCoverageInformation()
    {
        return $this->codeCoverage !== null;
    }

    /**
     * Runs a TestCase.
     *
     * @param PHPUnit_Framework_Test $test
     */
    public function run(PHPUnit_Framework_Test $test)
    {

      $test->resetCount();

        $error      = false;
        $failure    = false;
        $warning    = false;
        $incomplete = false;
        $risky      = false;
        $skipped    = false;

        $this->startTest($test);

        $errorHandlerSet = false;

        if ($this->convertErrorsToExceptions) {
            $oldErrorHandler = set_error_handler(
                ['PHPUnit_Util_ErrorHandler', 'handleError'],
                E_ALL | E_STRICT
            );

            if ($oldErrorHandler === null) {
                $errorHandlerSet = true;
            } else {
                restore_error_handler();
            }
        }

        $collectCodeCoverage = $this->codeCoverage !== null &&
                               !$test instanceof PHPUnit_Framework_WarningTestCase;

        if ($collectCodeCoverage) {
            $this->codeCoverage->start($test);
        }

        $monitorFunctions = $this->beStrictAboutResourceUsageDuringSmallTests &&
                            !$test instanceof PHPUnit_Framework_WarningTestCase &&
                            $test->getSize() == PHPUnit_Util_Test::SMALL &&
                            function_exists('xdebug_start_function_monitor');

        if ($monitorFunctions) {
            xdebug_start_function_monitor(ResourceOperations::getFunctions());
        }

        PHP_Timer::start();

        try {
                $test->runBare();

        } catch (PHPUnit_Framework_MockObject_Exception $e) {
            $e = new WarningException(
                $e->getMessage()
            );

            $warning = true;
        } catch (InvalidArgumentException $e) {
          $error = true;
        } catch (SkippedException $e) {
          $failure = true;
          $skipped = true;
        } catch (RiskyException $e) {
          $failure = true;
          $risky = true;
        } catch (IncompleteException $e) {
          $failure = true;
          $incomplete = true;
        } catch (WarningException $e) {
          $warning = true;
        } catch (PHPUnit_Framework_Exception $e) {
          $error = true;
        } catch (AssertionFailedException $e) {
          $failure = true;
        } catch (ExpectationFailedException $e) {
          $failure = true;
        } catch (Throwable $e) {
            $e     = new PHPUnit_Framework_ExceptionWrapper($e);
            $error = true;
        } catch (Exception $e) {
          // var_dump('TestResult::Exception trap');
          // var_dump(get_class($e));
          // var_dump($e->getMessage());
          // // var_dump(debug_backtrace(2));
          // var_dump($e->getTrace());
          // exit();
            $e     = new PHPUnit_Framework_ExceptionWrapper($e);
            $error = true;
        }

        $time = PHP_Timer::stop();
        // $test->addToAssertionCount(PHPUnit_Framework_Assert::getCount());
        $test->addToAssertionCount($test->getCount());

        if ($monitorFunctions) {
            $blacklist = new PHPUnit_Util_Blacklist;
            $functions = xdebug_get_monitored_functions();
            xdebug_stop_function_monitor();

            foreach ($functions as $function) {
                if (!$blacklist->isBlacklisted($function['filename'])) {
                    $this->addFailure(
                        $test,
                        new PHPUnit_Framework_RiskyTestError(
                            sprintf(
                                '%s() used in %s:%s',
                                $function['function'],
                                $function['filename'],
                                $function['lineno']
                            )
                        ),
                        $time
                    );
                }
            }
        }

        if ($this->beStrictAboutTestsThatDoNotTestAnything &&
            $test->getNumAssertions() == 0) {
            $risky = true;
        }

        if ($collectCodeCoverage) {
            $append           = !$risky && !$incomplete && !$skipped;
            $linesToBeCovered = [];
            $linesToBeUsed    = [];

            if ($append && ($test instanceof ZyngaTestCaseBase || $test instanceof PHPUnit_Framework_TestCase)) {
                try {

                    $linesToBeCovered = PHPUnit_Util_Test::getLinesToBeCovered(
                        $test->getClass(),
                        $test->getName(false)
                    );

                    $linesToBeUsed = PHPUnit_Util_Test::getLinesToBeUsed(
                        $test->getClass(),
                        $test->getName(false)
                    );

                } catch (PHPUnit_Framework_InvalidCoversTargetException $cce) {
                    $this->addWarning(
                        $test,
                        new WarningException(
                            $cce->getMessage()
                        ),
                        $time
                    );
                }
            }

            try {
                $this->codeCoverage->stop(
                    $append,
                    $linesToBeCovered,
                    $linesToBeUsed
                );
            } catch (UnintentionallyCoveredCodeException $cce) {
                if (!$test->isMedium() && !$test->isLarge()) {
                    $this->addFailure(
                        $test,
                        new PHPUnit_Framework_UnintentionallyCoveredCodeError(
                            'This test executed code that is not listed as code to be covered or used:' .
                            PHP_EOL . $cce->getMessage()
                        ),
                        $time
                    );
                }
            } catch (CoveredCodeNotExecutedException $cce) {
                $this->addFailure(
                    $test,
                    new PHPUnit_Framework_CoveredCodeNotExecutedException(
                        'This test did not execute all the code that is listed as code to be covered:' .
                        PHP_EOL . $cce->getMessage()
                    ),
                    $time
                );
            } catch (MissingCoversAnnotationException $cce) {
                $this->addFailure(
                    $test,
                    new PHPUnit_Framework_MissingCoversAnnotationException(
                        'This test does not have a @covers annotation but is expected to have one'
                    ),
                    $time
                );
            } catch (CodeCoverageException $cce) {
                $error = true;

                if (!isset($e)) {
                    $e = $cce;
                }
            } catch ( Exception $ue ) {
              $error = true;
              $e = $ue;
              // --
              // JEO: frame debugging.
              // --
              // $frame = 0;
              //foreach ( $e->getTrace() as $traceItem ) {
              //  echo "Trace frame=$frame\n";
              //  var_dump($traceItem);
              //  $frame++;
              //}
              //var_dump($e);
            }
        }

        if ($errorHandlerSet === true) {
            restore_error_handler();
        }


        // if ( preg_match('/ThrowExceptionTestCase/', get_class($test)) && $test->getName(false) == 'test' ) {
        //   var_dump('ThrowExceptionTestCase::trap');
        //   var_dump($test->getName(false));
        //   var_dump(get_class($e));
        //   var_dump($e->getTrace());
        //   var_dump($error);
        //   var_dump($failure);
        //   var_dump($skipped);
        //   exit();
        // }
        if ($error === true) {
            $this->addError($test, $e, $time);
        } elseif ($failure === true) {
            $this->addFailure($test, $e, $time);
        } elseif ($warning === true) {
            $this->addWarning($test, $e, $time);
        } elseif ($this->beStrictAboutTestsThatDoNotTestAnything &&
                 $test->getNumAssertions() == 0) {
            $this->addFailure(
                $test,
                new PHPUnit_Framework_RiskyTestError(
                    'This test did not perform any assertions'
                ),
                $time
            );
        } elseif ($this->beStrictAboutOutputDuringTests && $test->hasOutput()) {
            $this->addFailure(
                $test,
                new PHPUnit_Framework_OutputError(
                    sprintf(
                        'This test printed output: %s',
                        $test->getActualOutput()
                    )
                ),
                $time
            );
        } elseif ($this->beStrictAboutTodoAnnotatedTests && $test instanceof PHPUnit_Framework_TestCase) {
            $annotations = $test->getAnnotations();

            if (isset($annotations['method']['todo'])) {
                $this->addFailure(
                    $test,
                    new PHPUnit_Framework_RiskyTestError(
                        'Test method is annotated with @todo'
                    ),
                    $time
                );
            }
        }

        $this->endTest($test, $time);

    }

    /**
     * Gets the number of run tests.
     *
     * @return int
     */
    public function count()
    {
        return $this->runTests;
    }

    /**
     * Checks whether the test run should stop.
     *
     * @return bool
     */
    public function shouldStop()
    {
        return $this->stop;
    }

    /**
     * Marks that the test run should stop.
     */
    public function stop()
    {
        $this->stop = true;
    }

    /**
     * Returns the code coverage object.
     *
     * @return CodeCoverage
     *
     * @since Method available since Release 3.5.0
     */
    public function getCodeCoverage()
    {
        return $this->codeCoverage;
    }

    /**
     * Sets the code coverage object.
     *
     * @param CodeCoverage $codeCoverage
     *
     * @since Method available since Release 3.6.0
     */
    public function setCodeCoverage(CodeCoverage $codeCoverage)
    {
        $this->codeCoverage = $codeCoverage;
    }

    /**
     * Enables or disables the error-to-exception conversion.
     *
     * @param bool $flag
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 3.2.14
     */
    public function convertErrorsToExceptions($flag)
    {
        if (!is_bool($flag)) {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'boolean');
        }

        $this->convertErrorsToExceptions = $flag;
    }

    /**
     * Returns the error-to-exception conversion setting.
     *
     * @return bool
     *
     * @since Method available since Release 3.4.0
     */
    public function getConvertErrorsToExceptions()
    {
        return $this->convertErrorsToExceptions;
    }

    /**
     * Enables or disables the stopping when an error occurs.
     *
     * @param bool $flag
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 3.5.0
     */
    public function stopOnError($flag)
    {
        if (!is_bool($flag)) {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'boolean');
        }

        $this->stopOnError = $flag;
    }

    /**
     * Enables or disables the stopping when a failure occurs.
     *
     * @param bool $flag
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 3.1.0
     */
    public function stopOnFailure($flag)
    {
        if (!is_bool($flag)) {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'boolean');
        }

        $this->stopOnFailure = $flag;
    }

    /**
     * Enables or disables the stopping when a warning occurs.
     *
     * @param bool $flag
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 5.1.0
     */
    public function stopOnWarning($flag)
    {
        if (!is_bool($flag)) {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'boolean');
        }

        $this->stopOnWarning = $flag;
    }

    /**
     * @param bool $flag
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 4.0.0
     */
    public function beStrictAboutTestsThatDoNotTestAnything($flag)
    {
        if (!is_bool($flag)) {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'boolean');
        }

        $this->beStrictAboutTestsThatDoNotTestAnything = $flag;
    }

    /**
     * @return bool
     *
     * @since Method available since Release 4.0.0
     */
    public function isStrictAboutTestsThatDoNotTestAnything()
    {
        return $this->beStrictAboutTestsThatDoNotTestAnything;
    }

    /**
     * @param bool $flag
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 4.0.0
     */
    public function beStrictAboutOutputDuringTests($flag)
    {
        if (!is_bool($flag)) {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'boolean');
        }

        $this->beStrictAboutOutputDuringTests = $flag;
    }

    /**
     * @return bool
     *
     * @since Method available since Release 4.0.0
     */
    public function isStrictAboutOutputDuringTests()
    {
        return $this->beStrictAboutOutputDuringTests;
    }

    /**
     * @param bool $flag
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 5.0.0
     */
    public function beStrictAboutResourceUsageDuringSmallTests($flag)
    {
        if (!is_bool($flag)) {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'boolean');
        }

        $this->beStrictAboutResourceUsageDuringSmallTests = $flag;
    }

    /**
     * @return bool
     *
     * @since Method available since Release 5.0.0
     */
    public function isStrictAboutResourceUsageDuringSmallTests()
    {
        return $this->beStrictAboutResourceUsageDuringSmallTests;
    }

    /**
     * @param bool $flag
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 5.0.0
     */
    public function enforceTimeLimit($flag)
    {
        if (!is_bool($flag)) {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'boolean');
        }

        $this->enforceTimeLimit = $flag;
    }

    /**
     * @return bool
     *
     * @since Method available since Release 5.0.0
     */
    public function enforcesTimeLimit()
    {
        return $this->enforceTimeLimit;
    }

    /**
     * @param bool $flag
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 4.2.0
     */
    public function beStrictAboutTodoAnnotatedTests($flag)
    {
        if (!is_bool($flag)) {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'boolean');
        }

        $this->beStrictAboutTodoAnnotatedTests = $flag;
    }

    /**
     * @return bool
     *
     * @since Method available since Release 4.2.0
     */
    public function isStrictAboutTodoAnnotatedTests()
    {
        return $this->beStrictAboutTodoAnnotatedTests;
    }

    /**
     * Enables or disables the stopping for risky tests.
     *
     * @param bool $flag
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 4.0.0
     */
    public function stopOnRisky($flag)
    {
        if (!is_bool($flag)) {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'boolean');
        }

        $this->stopOnRisky = $flag;
    }

    /**
     * Enables or disables the stopping for incomplete tests.
     *
     * @param bool $flag
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 3.5.0
     */
    public function stopOnIncomplete($flag)
    {
        if (!is_bool($flag)) {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'boolean');
        }

        $this->stopOnIncomplete = $flag;
    }

    /**
     * Enables or disables the stopping for skipped tests.
     *
     * @param bool $flag
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 3.1.0
     */
    public function stopOnSkipped($flag)
    {
        if (!is_bool($flag)) {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'boolean');
        }

        $this->stopOnSkipped = $flag;
    }

    /**
     * Returns the time spent running the tests.
     *
     * @return float
     */
    public function time()
    {
        return $this->time;
    }

    /**
     * Returns whether the entire test was successful or not.
     *
     * @return bool
     */
    public function wasSuccessful()
    {
        return empty($this->errors) && empty($this->failures) && empty($this->warnings);
    }

    /**
     * Sets the timeout for small tests.
     *
     * @param int $timeout
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 3.6.0
     */
    public function setTimeoutForSmallTests($timeout)
    {
        if (!is_integer($timeout)) {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'integer');
        }

        $this->timeoutForSmallTests = $timeout;
    }

    /**
     * Sets the timeout for medium tests.
     *
     * @param int $timeout
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 3.6.0
     */
    public function setTimeoutForMediumTests($timeout)
    {
        if (!is_integer($timeout)) {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'integer');
        }

        $this->timeoutForMediumTests = $timeout;
    }

    /**
     * Sets the timeout for large tests.
     *
     * @param int $timeout
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 3.6.0
     */
    public function setTimeoutForLargeTests($timeout)
    {
        if (!is_integer($timeout)) {
            throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'integer');
        }

        $this->timeoutForLargeTests = $timeout;
    }

    /**
     * Returns the set timeout for large tests.
     *
     * @return int
     */
    public function getTimeoutForLargeTests()
    {
        return $this->timeoutForLargeTests;
    }


}
