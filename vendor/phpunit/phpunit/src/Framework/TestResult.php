<?hh // partial

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
use Zynga\Framework\Dynamic\V1\DynamicMethodCall;

use Zynga\PHPUnit\V2\Interfaces\TestInterface;
use Zynga\PHPUnit\V2\Interfaces\TestListenerInterface;
use Zynga\PHPUnit\V2\TestCase;
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
class PHPUnit_Framework_TestResult extends TestResult implements Countable {

  /**
   * @var array
   */
  protected $passed = [];

  /**
   * @var int
   */
  protected $runTests = 0;

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
   * Informs the result that a testsuite will be started.
   *
   * @param PHPUnit_Framework_TestSuite $suite
   *
   * @since Method available since Release 2.2.0
   */
  public function startTestSuite(TestInterface $suite) {
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
  public function endTestSuite(TestInterface $suite) {
    $this->listeners()->endTestSuite($suite);
  }

  /**
   * Informs the result that a test will be started.
   *
   * @param TestInterface $test
   */
  public function startTest(TestInterface $test) {
    $this->setLastTestFailed(false);

    $this->runTests += count($test);

    $this->listeners()->startTest($test);
  }

  /**
   * Informs the result that a test was completed.
   *
   * @param TestInterface $test
   * @param float                  $time
   */
  public function endTest(TestInterface $test, $time) {
    $this->listeners()->endTest($test, $time);

    if (!$this->getLastTestFailed() && $test instanceof TestCase) {
      $class = $test->getClass();
      $key = $class.'::'.$test->getName();

      $this->passed[$key] =
        ['result' => $test->getResult(), 'size' => $test->getSize()];

      $this->addToTime($time);
    }
  }

  /**
   * Returns the names of the tests that have passed.
   *
   * @return array
   *
   * @since Method available since Release 3.4.0
   */
  public function passed() {
    return $this->passed;
  }

  /**
   * Returns the (top) test suite.
   *
   * @return PHPUnit_Framework_TestSuite
   *
   * @since Method available since Release 3.0.0
   */
  public function topTestSuite() {
    return $this->topTestSuite;
  }

  /**
   * Returns whether code coverage information should be collected.
   *
   * @return bool If code coverage should be collected
   *
   * @since Method available since Release 3.2.0
   */
  public function getCollectCodeCoverageInformation() {
    return $this->codeCoverage !== null;
  }

  private bool $_errorHandlerSet = false;
  private mixed $_errorHandlerPrevious = null;

  public function run_handleErrorHandler(): void {
    if ($this->convertErrorsToExceptions) {
      $this->_errorHandlerPrevious = set_error_handler(
        ['PHPUnit_Util_ErrorHandler', 'handleError'],
        E_ALL | E_STRICT,
      );

      if ($this->_errorHandlerPrevious === null) {
        $this->_errorHandlerSet = true;
      } else {
        // clear the error handler in case of bad, or failed handlers as null
        // is reused to indicate a bad callback also (yay php).
        restore_error_handler();
      }
    }
  }

  public function run_handleRestoreErrorHandler(): void {
    if ($this->_errorHandlerSet == true) {
      restore_error_handler();
      $this->_errorHandlerPrevious = null;
    }
  }

  /**
   * Runs a TestCase.
   *
   * @param TestInterface $test
   */
  public function run(TestInterface $test) {

    // --
    // JEO: Come up with a better refinement method than this, pretty sure I am
    // fighting the type checker here for no solid reason.
    // --
    if (!$test instanceof TestCase) {
      return;
    }

    $test->resetCount();

    $e = null;
    $error = false;
    $failure = false;
    $warning = false;
    $incomplete = false;
    $risky = false;
    $skipped = false;

    $this->startTest($test);

    $this->run_handleErrorHandler();

    $collectCodeCoverage =
      $this->codeCoverage !== null &&
      !$test instanceof PHPUnit_Framework_WarningTestCase;

    if ($collectCodeCoverage) {
      $this->codeCoverage->start($test);
    }

    $monitorFunctions =
      $this->beStrictAboutResourceUsageDuringSmallTests &&
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
      $e = new WarningException($e->getMessage());

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
      $e = new PHPUnit_Framework_ExceptionWrapper($e);
      $error = true;
    } catch (Exception $e) {
      // var_dump('TestResult::Exception trap');
      // var_dump(get_class($e));
      // var_dump($e->getMessage());
      // // var_dump(debug_backtrace(2));
      // var_dump($e->getTrace());
      // exit();
      $e = new PHPUnit_Framework_ExceptionWrapper($e);
      $error = true;
    }

    $time = PHP_Timer::stop();
    // $test->addToAssertionCount(PHPUnit_Framework_Assert::getCount());
    $test->addToAssertionCount($test->getCount());

    if ($monitorFunctions) {
      $blacklist = new PHPUnit_Util_Blacklist();
      $functions = xdebug_get_monitored_functions();
      xdebug_stop_function_monitor();

      foreach ($functions as $function) {
        if (!$blacklist->isBlacklisted($function['filename'])) {
          $this->addFailure(
            $test,
            new RiskyException(
              sprintf(
                '%s() used in %s:%s',
                $function['function'],
                $function['filename'],
                $function['lineno'],
              ),
            ),
            $time,
          );
        }
      }
    }

    if ($this->beStrictAboutTestsThatDoNotTestAnything &&
        $test->getNumAssertions() == 0) {
      $risky = true;
    }

    if ($collectCodeCoverage) {
      $append = !$risky && !$incomplete && !$skipped;
      $linesToBeCovered = [];
      $linesToBeUsed = [];

      if ($append &&
          ($test instanceof ZyngaTestCaseBase || $test instanceof TestCase)) {
        try {

          $linesToBeCovered = PHPUnit_Util_Test::getLinesToBeCovered(
            $test->getClass(),
            $test->getName(),
          );

          $linesToBeUsed = PHPUnit_Util_Test::getLinesToBeUsed(
            $test->getClass(),
            $test->getName(),
          );

        } catch (PHPUnit_Framework_InvalidCoversTargetException $cce) {
          $this->addWarning(
            $test,
            new WarningException($cce->getMessage()),
            $time,
          );
        }
      }

      try {
        $this->codeCoverage->stop($append, $linesToBeCovered, $linesToBeUsed);
      } catch (UnintentionallyCoveredCodeException $cce) {
        if (!$test->isMedium() && !$test->isLarge()) {
          $this->addFailure(
            $test,
            new PHPUnit_Framework_UnintentionallyCoveredCodeError(
              'This test executed code that is not listed as code to be covered or used:'.
              PHP_EOL.
              $cce->getMessage(),
            ),
            $time,
          );
        }
      } catch (CoveredCodeNotExecutedException $cce) {
        $this->addFailure(
          $test,
          new PHPUnit_Framework_CoveredCodeNotExecutedException(
            'This test did not execute all the code that is listed as code to be covered:'.
            PHP_EOL.
            $cce->getMessage(),
          ),
          $time,
        );
      } catch (MissingCoversAnnotationException $cce) {
        $this->addFailure(
          $test,
          new PHPUnit_Framework_MissingCoversAnnotationException(
            'This test does not have a @covers annotation but is expected to have one',
          ),
          $time,
        );
      } catch (Exception $ue) {
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

    $this->run_handleRestoreErrorHandler();

    // if ($e instanceof Exception) {
    //   var_dump('ThrowExceptionTestResult::trap');
    //   var_dump('name='.$test->getName(false));
    //   var_dump('class='.get_class($e));
    //   var_dump('trace=');
    //   var_dump($e->getTrace());
    //
    //   var_dump('error='.$error);
    //   var_dump('failure='.$failure);
    //   var_dump('skipped='.$skipped);
    //   exit();
    // }

    if ($error === true && !$e instanceof Exception) {
      error_log(
        'JEO WARNING - NON-Exception based error thrown e='.gettype($e),
      );
    } else if ($error === true && $e instanceof Exception) {
      $this->addError($test, $e, $time);
    } else if ($failure === true && $e instanceof Exception) {
      $this->addFailure($test, $e, $time);
    } else if ($warning === true && $e instanceof Exception) {
      $this->addWarning($test, $e, $time);
    } else if ($this->beStrictAboutTestsThatDoNotTestAnything &&
               $test->getNumAssertions() == 0) {
      $this->addFailure(
        $test,
        new RiskyException('This test did not perform any assertions'),
        $time,
      );
    } else if ($this->beStrictAboutOutputDuringTests && $test->hasOutput()) {
      $this->addFailure(
        $test,
        new PHPUnit_Framework_OutputError(
          sprintf('This test printed output: %s', $test->getActualOutput()),
        ),
        $time,
      );
    } else if ($this->beStrictAboutTodoAnnotatedTests &&
               $test instanceof TestCase) {
      $annotations = $test->getAnnotations();

      if (isset($annotations['method']['todo'])) {
        $this->addFailure(
          $test,
          new RiskyException('Test method is annotated with @todo'),
          $time,
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
  public function count(): int {
    return $this->runTests;
  }

  /**
   * Returns the code coverage object.
   *
   * @return CodeCoverage
   *
   * @since Method available since Release 3.5.0
   */
  public function getCodeCoverage() {
    return $this->codeCoverage;
  }

  /**
   * Sets the code coverage object.
   *
   * @param CodeCoverage $codeCoverage
   *
   * @since Method available since Release 3.6.0
   */
  public function setCodeCoverage(CodeCoverage $codeCoverage) {
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
  public function convertErrorsToExceptions($flag) {
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
  public function getConvertErrorsToExceptions() {
    return $this->convertErrorsToExceptions;
  }

  /**
   * @param bool $flag
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since Method available since Release 4.0.0
   */
  public function beStrictAboutTestsThatDoNotTestAnything($flag) {
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
  public function isStrictAboutTestsThatDoNotTestAnything() {
    return $this->beStrictAboutTestsThatDoNotTestAnything;
  }

  /**
   * @param bool $flag
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since Method available since Release 4.0.0
   */
  public function beStrictAboutOutputDuringTests($flag) {
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
  public function isStrictAboutOutputDuringTests() {
    return $this->beStrictAboutOutputDuringTests;
  }

  /**
   * @param bool $flag
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since Method available since Release 5.0.0
   */
  public function beStrictAboutResourceUsageDuringSmallTests($flag) {
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
  public function isStrictAboutResourceUsageDuringSmallTests() {
    return $this->beStrictAboutResourceUsageDuringSmallTests;
  }

  /**
   * @param bool $flag
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since Method available since Release 5.0.0
   */
  public function enforceTimeLimit($flag) {
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
  public function enforcesTimeLimit() {
    return $this->enforceTimeLimit;
  }

  /**
   * @param bool $flag
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since Method available since Release 4.2.0
   */
  public function beStrictAboutTodoAnnotatedTests($flag) {
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
  public function isStrictAboutTodoAnnotatedTests() {
    return $this->beStrictAboutTodoAnnotatedTests;
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
  public function setTimeoutForSmallTests($timeout) {
    if (!is_int($timeout)) {
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
  public function setTimeoutForMediumTests(int $timeout): void {
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
  public function setTimeoutForLargeTests(int $timeout): void {
    $this->timeoutForLargeTests = $timeout;
  }

  /**
   * Returns the set timeout for large tests.
   *
   * @return int
   */
  public function getTimeoutForLargeTests() {
    return $this->timeoutForLargeTests;
  }

}
