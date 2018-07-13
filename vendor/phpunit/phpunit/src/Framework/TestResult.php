<?hh

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */
use PHPUnit\Exceptions\AssertionFailedError;
use PHPUnit\Exceptions\CoveredCodeNotExecutedException;
use PHPUnit\Exceptions\Exception as PHPUnit_Exceptions_Exception;
use PHPUnit\Exceptions\ExceptionWrapper;
use PHPUnit\Exceptions\IncompleteTestError;
use PHPUnit\Exceptions\InvalidCoversTargetException;
use PHPUnit\Exceptions\MissingCoversAnnotationException;
use PHPUnit\Exceptions\OutputError;
use PHPUnit\Exceptions\Error\RiskyTestError;
use PHPUnit\Exceptions\SkippedTestError;
use PHPUnit\Exceptions\Warning;
use PHPUnit\Exceptions\UnintentionallyCoveredCodeError;
use PHPUnit\Framework\WarningTestCase;
use PHPUnit\Interfaces\SkippedTest;
use PHPUnit\Interfaces\IncompleteTest;
use PHPUnit\Interfaces\TestInterface;
use PHPUnit\Interfaces\TestListener;
use PHPUnit\Util\UtilInvalidArgumentHelper;
use PHPUnit\Util\UtilTest;

use SebastianBergmann\CodeCoverage\CodeCoverage;
use SebastianBergmann\CodeCoverage\Exception as CodeCoverageException;
// JEO: Commented out these duplicated? Exceptions?
//use SebastianBergmann\CodeCoverage\CoveredCodeNotExecutedException;
//use SebastianBergmann\CodeCoverage\MissingCoversAnnotationException;
use SebastianBergmann\CodeCoverage\UnintentionallyCoveredCodeException;
use SebastianBergmann\ResourceOperations\ResourceOperations;
use Zynga\Framework\Testing\TestCase\V2\Base as ZyngaTestCaseBase;

/**
 * A TestResult collects the results of executing a test case.
 *
 * @since Class available since Release 2.0.0
 */
class PHPUnit_Framework_TestResult implements Countable {

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
     * @var array
     */
    protected $listeners = [];

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
     * @var bool
     */
    private $registerMockObjectsFromTestArgumentsRecursively = false;

    /**
     * Registers a TestListener.
     *
     * @param  TestListener
     */
    public function addListener(TestListener $listener)
    {
        $this->listeners[] = $listener;
    }

    /**
     * Unregisters a TestListener.
     *
     * @param TestListener $listener
     */
    public function removeListener(TestListener $listener)
    {
        foreach ($this->listeners as $key => $_listener) {
            if ($listener === $_listener) {
                unset($this->listeners[$key]);
            }
        }
    }

    /**
     * Flushes all flushable TestListeners.
     *
     * @since Method available since Release 3.0.0
     */
    public function flushListeners()
    {
        foreach ($this->listeners as $listener) {
            if ($listener instanceof PHPUnit_Util_Printer) {
                $listener->flush();
            }
        }
    }

    /**
     * Adds an error to the list of errors.
     *
     * @param TestInterface $test
     * @param Throwable              $t
     * @param float                  $time
     */
    public function addError(TestInterface $test, $t, float $time) {
        if ($t instanceof PHPUnit_Framework_RiskyTest) {
            $this->risky[] = new PHPUnit_Framework_TestFailure($test, $t);
            $notifyMethod  = 'addRiskyTest';

            if ($this->stopOnRisky) {
                $this->stop();
            }
        } elseif ($t instanceof IncompleteTest) {
            $this->notImplemented[] = new PHPUnit_Framework_TestFailure($test, $t);
            $notifyMethod           = 'addIncompleteTest';

            if ($this->stopOnIncomplete) {
                $this->stop();
            }
        } elseif ($t instanceof SkippedTest) {
            $this->skipped[] = new PHPUnit_Framework_TestFailure($test, $t);
            $notifyMethod    = 'addSkippedTest';

            if ($this->stopOnSkipped) {
                $this->stop();
            }
        } else {
            $this->errors[] = new PHPUnit_Framework_TestFailure($test, $t);
            $notifyMethod   = 'addError';

            if ($this->stopOnError || $this->stopOnFailure) {
                $this->stop();
            }
        }

        // @see https://github.com/sebastianbergmann/phpunit/issues/1953
        if ($t instanceof Error) {
            $t = new ExceptionWrapper($t);
        }

        foreach ($this->listeners as $listener) {
            $listener->$notifyMethod($test, $t, $time);
        }

        $this->lastTestFailed = true;
        $this->time          += $time;
    }

    /**
     * Adds a warning to the list of warnings.
     * The passed in exception caused the warning.
     *
     * @param TestInterface    $test
     * @param Warning $e
     * @param float                     $time
     *
     * @since Method available since Release 5.1.0
     */
    public function addWarning(TestInterface $test, Warning $e, $time)
    {
        if ($this->stopOnWarning) {
            $this->stop();
        }

        $this->warnings[] = new PHPUnit_Framework_TestFailure($test, $e);

        foreach ($this->listeners as $listener) {
            // @todo Remove check for PHPUnit 6.0.0
            // @see  https://github.com/sebastianbergmann/phpunit/pull/1840#issuecomment-162535997
            if (method_exists($listener, 'addWarning')) {
                $listener->addWarning($test, $e, $time);
            }
        }

        $this->time += $time;
    }

    private function _addFailureHandleRiskyTest(TestInterface $test, Exception $e, float $time): void {

      $this->risky[] = new PHPUnit_Framework_TestFailure($test, $e);

      if ($this->stopOnRisky) {
          $this->stop();
      }

      foreach ($this->listeners as $listener) {
          $listener->addRiskyTest($test, $e, $time);
      }

    }

    private function _addFailureHandleIncompleteTest(TestInterface $test, Exception $e, float $time): void {

      $this->notImplemented[] = new PHPUnit_Framework_TestFailure($test, $e);

      if ($this->stopOnIncomplete) {
          $this->stop();
      }

      foreach ($this->listeners as $listener) {
        $listener->addIncompleteTest($test, $e, $time);
      }

    }

    private function _addFailureHandleSkippedTest(TestInterface $test, Exception $e, float $time): void {

      $this->skipped[] = new PHPUnit_Framework_TestFailure($test, $e);

      if ($this->stopOnSkipped) {
        $this->stop();
      }

      foreach ($this->listeners as $listener) {
        $listener->addSkippedTest($test, $e, $time);
      }

    }

    private function _addFailureHandleFailedTest(TestInterface $test, Exception $e, float $time): void {

      $this->failures[] = new PHPUnit_Framework_TestFailure($test, $e);

      if ($this->stopOnFailure) {
          $this->stop();
      }

      foreach ($this->listeners as $listener) {
        $listener->addFailure($test, $e, $time);
      }

    }

    /**
     * Adds a failure to the list of failures.
     * The passed in exception caused the failure.
     *
     * @param PHPUnit_Framework_Test                 $test
     * @param AssertionFailedError $e
     * @param float                                  $time
     */
    public function addFailure(TestInterface $test, Exception $e, float $time) {
        if ($e instanceof RiskyTestError) {
          $this->_addFailureHandleRiskyTest($test, $e, $time);
        } elseif ($e instanceof OutputError) {
          $this->_addFailureHandleRiskyTest($test, $e, $time);
        } elseif ($e instanceof IncompleteTestError) {
          $this->_addFailureHandleIncompleteTest($test, $e, $time);
        } elseif ($e instanceof SkippedTestError) {
          $this->_addFailureHandleSkippedTest($test, $e, $time);
        } else {
          $this->_addFailureHandleFailedTest($test, $e, $time);
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

        foreach ($this->listeners as $listener) {
            $listener->startTestSuite($suite);
        }
    }

    /**
     * Informs the result that a testsuite was completed.
     *
     * @param PHPUnit_Framework_TestSuite $suite
     *
     * @since Method available since Release 2.2.0
     */
    public function endTestSuite(PHPUnit_Framework_TestSuite $suite) {
      foreach ($this->listeners as $listener) {
        $listener->endTestSuite($suite);
      }
    }

    /**
     * Informs the result that a test will be started.
     *
     * @param PHPUnit_Framework_Test $test
     */
    public function startTest(TestInterface $test): void {
      $this->lastTestFailed = false;
      $this->runTests      += count($test);

      foreach ($this->listeners as $listener) {
        $listener->startTest($test);
      }

    }

    /**
     * Informs the result that a test was completed.
     *
     * @param PHPUnit_Framework_Test $test
     * @param float                  $time
     */
    public function endTest(TestInterface $test, $time): void {

      foreach ($this->listeners as $listener) {
        $listener->endTest($test, $time);
      }

      if (!$this->lastTestFailed && $test instanceof TestInterface) {
        $class  = get_class($test);
        $key    = $class . '::' . $test->getName();

        $this->passed[$key] = [
          'result' => $test->getResult(),
          'size'   => UtilTest::getSize(
            $class,
            $test->getName()
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
    public function allHarmless(): bool {
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
    public function run(TestInterface $test) {

      $test->resetCount();      

      $test->setRegisterMockObjectsFromTestArgumentsRecursively(
        $this->registerMockObjectsFromTestArgumentsRecursively
      );

        $e = null;
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
                               !$test instanceof WarningTestCase;

        if ($collectCodeCoverage) {
            $this->codeCoverage->start($test);
        }

        $monitorFunctions = $this->beStrictAboutResourceUsageDuringSmallTests &&
                            !$test instanceof WarningTestCase &&
                            $test->getSize() == UtilTest::SMALL &&
                            function_exists('xdebug_start_function_monitor');

        if ($monitorFunctions) {
            xdebug_start_function_monitor(ResourceOperations::getFunctions());
        }

        PHP_Timer::start();

        try {
            if (!$test instanceof WarningTestCase &&
              $test->getSize() != UtilTest::UNKNOWN &&
                $this->enforceTimeLimit &&
                extension_loaded('pcntl') && class_exists('PHP_Invoker')) {
                switch ($test->getSize()) {
                    case UtilTest::SMALL:
                        $_timeout = $this->timeoutForSmallTests;
                        break;

                    case UtilTest::MEDIUM:
                        $_timeout = $this->timeoutForMediumTests;
                        break;

                    case UtilTest::LARGE:
                        $_timeout = $this->timeoutForLargeTests;
                        break;
                }

                $invoker = new PHP_Invoker();
                $invoker->invoke([$test, 'runBare'], [], $_timeout);
            } else {
              $test->runBare();
            }

        } catch (PHPUnit_Framework_MockObject_Exception $e) {
            $e = new Warning(
                $e->getMessage()
            );

            $warning = true;
        } catch (AssertionFailedError $e) {
            $failure = true;

            if ($e instanceof RiskyTestError) {
                $risky = true;
            } elseif ($e instanceof IncompleteTestError) {
                $incomplete = true;
            } elseif ($e instanceof SkippedTestError) {
                $skipped = true;
            }
        } catch (Warning $e) {
            $warning = true;
        } catch (PHPUnit_Exceptions_Exception $e) {
            $error = true;
        } catch (Throwable $e) {
            $e     = new ExceptionWrapper($e);
            $error = true;
        } catch (Exception $e) {
            $e     = new ExceptionWrapper($e);
            $error = true;
        }

        $time = PHP_Timer::stop();

        $test->addToAssertionCount($test->getCount());

        if ($monitorFunctions) {
            $blacklist = new PHPUnit_Util_Blacklist();
            $functions = xdebug_get_monitored_functions();
            xdebug_stop_function_monitor();

            foreach ($functions as $function) {
                if (!$blacklist->isBlacklisted($function['filename'])) {
                    $this->addFailure(
                        $test,
                        new RiskyTestError(
                            sprintf('%s() used', $function['function']),
                            0,
                            $function['filename'],
                            $function['lineno']
                        ),
                        $time
                    );
                }
            }
        }

        if ($this->beStrictAboutTestsThatDoNotTestAnything && $test->getNumAssertions() == 0) {
          $risky = true;
        }

        if ($collectCodeCoverage) {
            $append           = !$risky && !$incomplete && !$skipped;
            $linesToBeCovered = Map {};
            $linesToBeUsed    = Map {};

            if ($append && $test instanceof TestInterface) {
                try {

                    $linesToBeCovered = UtilTest::getLinesToBeCovered(
                        get_class($test),
                        $test->getName()
                    );

                    $linesToBeUsed = UtilTest::getLinesToBeUsed(
                        get_class($test),
                        $test->getName()
                    );

                } catch (InvalidCoversTargetException $cce) {
                    $this->addWarning(
                        $test,
                        new Warning(
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
                  //string $file, int $line, ?Exception $previous = null
                    $this->addFailure(
                        $test,
                        new UnintentionallyCoveredCodeError(
                            'This test executed code that is not listed as code to be covered or used:' .
                            PHP_EOL . $cce->getMessage(),
                            $cce->getCode(),
                            $cce->getFile(),
                            $cce->getLine(),
                            $cce
                        ),
                        $time
                    );
                }
            } catch (CoveredCodeNotExecutedException $cce) {
                $this->addFailure(
                    $test,
                    new CoveredCodeNotExecutedException(
                        'This test did not execute all the code that is listed as code to be covered:' .
                        PHP_EOL . $cce->getMessage(),
                        $cce->getCode(),
                        $cce->getFile(),
                        $cce->getLine(),
                        $cce
                    ),
                    $time
                );
            } catch (MissingCoversAnnotationException $cce) {
                $this->addFailure(
                    $test,
                    new MissingCoversAnnotationException(
                      'This test does not have a @covers annotation but is expected to have one',
                      $cce->getCode(),
                      $cce->getFile(),
                      $cce->getLine(),
                      $cce
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
            }
        }

        if ($errorHandlerSet === true) {
            restore_error_handler();
        }

        if ($error === true && $e instanceof Error) {
            $this->addError($test, $e, $time);
        } elseif ($failure === true && $e instanceof Failure) {
            $this->addFailure($test, $e, $time);
        } elseif ($warning === true && $e instanceof Warning) {
            $this->addWarning($test, $e, $time);
        }

        $this->_handleStrictTestsWithNoAssertions($test, $time);
        $this->_handleStrictOutputDuringTests($test, $time);
        $this->_handleStrictTodoAnnotation($test, $time);

        $this->endTest($test, $time);

    }

    private function _handleStrictTestsWithNoAssertions(TestInterface $test, float $time) {
      if ($this->beStrictAboutTestsThatDoNotTestAnything !== true) {
        return;
      }
      if ($test->getNumAssertions() == 0) {
        $this->addFailure(
          $test,
          new RiskyTestError(
              'This test did not perform any assertions',
              255,
              $test->getName(),
              0
          ),
          $time
        );
      }
    }
    private function _handleStrictOutputDuringTests(TestInterface $test, float $time): void {

      if ( $this->beStrictAboutOutputDuringTests !== true ) {
        return;
      }

      if ( $test->hasOutput() ) {
        $this->addFailure( $test,
          new OutputError(
              sprintf( 'This test printed output: %s', $test->getActualOutput())
          ),
          $time
        );
      }
    }

    private function _handleStrictTodoAnnotation(TestInterface $test, float $time): void {

      if ($this->beStrictAboutTodoAnnotatedTests !== true) {
        return;
      }

      $annotations = $test->getAnnotations();

      if ($annotations->methodAnnotations->containsKey('todo')) {
          $this->addFailure(
              $test,
              new RiskyTestError(
                  'Test method is annotated with @todo',
                  255,
                  $test->getName(),
                  0
              ),
              $time
          );
      }
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
     * @throws PHPUnit_Exceptions_Exception
     *
     * @since Method available since Release 3.2.14
     */
    public function convertErrorsToExceptions($flag)
    {
        if (!is_bool($flag)) {
            throw UtilInvalidArgumentHelper::factory(1, 'boolean');
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
     * @throws PHPUnit_Exceptions_Exception
     *
     * @since Method available since Release 3.5.0
     */
    public function stopOnError($flag)
    {
        if (!is_bool($flag)) {
            throw UtilInvalidArgumentHelper::factory(1, 'boolean');
        }

        $this->stopOnError = $flag;
    }

    /**
     * Enables or disables the stopping when a failure occurs.
     *
     * @param bool $flag
     *
     * @throws PHPUnit_Exceptions_Exception
     *
     * @since Method available since Release 3.1.0
     */
    public function stopOnFailure($flag)
    {
        if (!is_bool($flag)) {
            throw UtilInvalidArgumentHelper::factory(1, 'boolean');
        }

        $this->stopOnFailure = $flag;
    }

    /**
     * Enables or disables the stopping when a warning occurs.
     *
     * @param bool $flag
     *
     * @throws PHPUnit_Exceptions_Exception
     *
     * @since Method available since Release 5.1.0
     */
    public function stopOnWarning($flag)
    {
        if (!is_bool($flag)) {
            throw UtilInvalidArgumentHelper::factory(1, 'boolean');
        }

        $this->stopOnWarning = $flag;
    }

    /**
     * @param bool $flag
     *
     * @throws PHPUnit_Exceptions_Exception
     *
     * @since Method available since Release 4.0.0
     */
    public function beStrictAboutTestsThatDoNotTestAnything($flag)
    {
        if (!is_bool($flag)) {
            throw UtilInvalidArgumentHelper::factory(1, 'boolean');
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
     * @throws PHPUnit_Exceptions_Exception
     *
     * @since Method available since Release 4.0.0
     */
    public function beStrictAboutOutputDuringTests($flag)
    {
        if (!is_bool($flag)) {
            throw UtilInvalidArgumentHelper::factory(1, 'boolean');
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
     * @throws PHPUnit_Exceptions_Exception
     *
     * @since Method available since Release 5.0.0
     */
    public function beStrictAboutResourceUsageDuringSmallTests($flag)
    {
        if (!is_bool($flag)) {
            throw UtilInvalidArgumentHelper::factory(1, 'boolean');
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
     * @throws PHPUnit_Exceptions_Exception
     *
     * @since Method available since Release 5.0.0
     */
    public function enforceTimeLimit($flag)
    {
        if (!is_bool($flag)) {
            throw UtilInvalidArgumentHelper::factory(1, 'boolean');
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
     * @throws PHPUnit_Exceptions_Exception
     *
     * @since Method available since Release 4.2.0
     */
    public function beStrictAboutTodoAnnotatedTests($flag)
    {
        if (!is_bool($flag)) {
            throw UtilInvalidArgumentHelper::factory(1, 'boolean');
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
     * @throws PHPUnit_Exceptions_Exception
     *
     * @since Method available since Release 4.0.0
     */
    public function stopOnRisky($flag)
    {
        if (!is_bool($flag)) {
            throw UtilInvalidArgumentHelper::factory(1, 'boolean');
        }

        $this->stopOnRisky = $flag;
    }

    /**
     * Enables or disables the stopping for incomplete tests.
     *
     * @param bool $flag
     *
     * @throws PHPUnit_Exceptions_Exception
     *
     * @since Method available since Release 3.5.0
     */
    public function stopOnIncomplete($flag)
    {
        if (!is_bool($flag)) {
            throw UtilInvalidArgumentHelper::factory(1, 'boolean');
        }

        $this->stopOnIncomplete = $flag;
    }

    /**
     * Enables or disables the stopping for skipped tests.
     *
     * @param bool $flag
     *
     * @throws PHPUnit_Exceptions_Exception
     *
     * @since Method available since Release 3.1.0
     */
    public function stopOnSkipped($flag)
    {
        if (!is_bool($flag)) {
            throw UtilInvalidArgumentHelper::factory(1, 'boolean');
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
     * @throws PHPUnit_Exceptions_Exception
     *
     * @since Method available since Release 3.6.0
     */
    public function setTimeoutForSmallTests($timeout) {
        if (!is_int($timeout)) {
            throw UtilInvalidArgumentHelper::factory(1, 'integer');
        }

        $this->timeoutForSmallTests = $timeout;
    }

    /**
     * Sets the timeout for medium tests.
     *
     * @param int $timeout
     *
     * @throws PHPUnit_Exceptions_Exception
     *
     * @since Method available since Release 3.6.0
     */
    public function setTimeoutForMediumTests($timeout) {
        if (!is_int($timeout)) {
            throw UtilInvalidArgumentHelper::factory(1, 'integer');
        }

        $this->timeoutForMediumTests = $timeout;
    }

    /**
     * Sets the timeout for large tests.
     *
     * @param int $timeout
     *
     * @throws PHPUnit_Exceptions_Exception
     *
     * @since Method available since Release 3.6.0
     */
    public function setTimeoutForLargeTests($timeout)
    {
        if (!is_int($timeout)) {
            throw UtilInvalidArgumentHelper::factory(1, 'integer');
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

    /**
     * @param bool $flag
     *
     * @since Method available since Release 5.4.0
     */
    public function setRegisterMockObjectsFromTestArgumentsRecursively($flag)
    {
        if (!is_bool($flag)) {
            throw UtilInvalidArgumentHelper::factory(1, 'boolean');
        }

        $this->registerMockObjectsFromTestArgumentsRecursively = $flag;
    }


    private function _getHierarchyAsReflection(string $className, bool $asReflectionObjects): Vector<ReflectionClass> {
      $classes = Vector {};

      $reflected = ReflectionClasses::getReflection($className);

      if ( $reflected instanceof ReflectionClass) {
        $classes->add($reflected);
      }

      $done = false;
      $class = null;
      while($done === false) {

        $lastItem = $classes->lastValue();

        if ( $lastItem instanceof ReflectionClass ) {

          $parent = $lastItem->getParentClass();

          if ( $parent instanceof ReflectionClass ) {
            $classes->add($parent);
          } else {
            $done = true;
          }

        }

      }

      return $classes;

    }

    private function _getHierarchyAsStrings(string $className, bool $asReflectionObjects): Vector<string> {
      $classes = Vector {};

      $classes->add($className);

      $done = false;
      $class = null;
      while($done === false) {

        $lastItemName = $classes->lastValue();

        $lastItem = ReflectionClasses::getReflection($lastItemName);

        if ( $lastItem instanceof ReflectionClass ) {

          $parent = $lastItem->getParentClass();

          if ( $parent instanceof ReflectionClass ) {
            $classes->add($parent->getName());
          } else {
            $done = true;
          }

        }

      }

      return $classes;
    }

    /**
     * Returns the class hierarchy for a given class.
     *
     * @param string $className
     * @param bool   $asReflectionObjects
     *
     * @return array
     */
    protected function getHierarchy(string $className, bool $asReflectionObjects = false): Vector<mixed> {
      $classes = Vector {};

      if ($asReflectionObjects) {
        $t_classes = $this->_getHierarchyAsReflection($className, $asReflectionObjects);
        foreach ($t_classes as $t_class) {
          $classes->add($t_class);
        }
      } else {
        $t_classes = $this->_getHierarchyAsStrings($className, $asReflectionObjects);
        foreach ($t_classes as $t_class) {
          $classes->add($t_class);
        }
      }

      return $classes;

    }
}
