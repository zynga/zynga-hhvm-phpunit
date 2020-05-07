<?hh // strict

namespace Zynga\PHPUnit\V2;

use SebastianBergmann\CodeCoverage\CodeCoverage;
use SebastianBergmann\CodeCoverage\CoveredCodeNotExecutedException;
use SebastianBergmann\CodeCoverage\MissingCoversAnnotationException;
use SebastianBergmann\CodeCoverage\UnintentionallyCoveredCodeException;
use SebastianBergmann\ResourceOperations\ResourceOperations;

use Zynga\Framework\Performance\V1\Tracker as PerformanceTracker;
use Zynga\Framework\ReflectionCache\V1\ReflectionClasses;
use Zynga\Framework\Testing\TestCase\V2\Base as ZyngaTestCaseBase;

use Zynga\PHPUnit\V2\Exceptions\AssertionFailedException;
use Zynga\PHPUnit\V2\Exceptions\ExpectationFailedException;
use Zynga\PHPUnit\V2\Exceptions\InvalidArgumentException;
use Zynga\PHPUnit\V2\Exceptions\WarningException;
use Zynga\PHPUnit\V2\Exceptions\TestError\IncompleteException;
use Zynga\PHPUnit\V2\Exceptions\TestError\RiskyException;
use Zynga\PHPUnit\V2\Exceptions\TestError\SkippedException;
use Zynga\PHPUnit\V2\IncompleteTestCase;
use Zynga\PHPUnit\V2\Interfaces\TestInterface;
use Zynga\PHPUnit\V2\Interfaces\TestListenerInterface;
use Zynga\PHPUnit\V2\Profiler\XDebug;
use Zynga\PHPUnit\V2\TestResult\Listeners;
use Zynga\PHPUnit\V2\TestResult\TestFailures;
use Zynga\PHPUnit\V2\TestCase\Size;
use Zynga\PHPUnit\V2\Exceptions\ExceptionWrapper;
use Zynga\PHPUnit\V2\WarningTestCase;

use \Exception;
use \PHPUnit_Framework_CoveredCodeNotExecutedException;
use \PHPUnit_Framework_Exception;
use \PHPUnit_Framework_InvalidCoversTargetException;
//use \PHPUnit_Framework_MockObject_Exception;
use \PHPUnit_Framework_MissingCoversAnnotationException;
use \PHPUnit_Framework_OutputError;
use \PHPUnit_Framework_WarningTestCase;
use \PHPUnit_Framework_UnintentionallyCoveredCodeError;
use \PHPUnit_Util_Blacklist;
use \PHPUnit_Util_Test;

class TestResult {
  private Listeners $_listeners;
  private TestFailures $_testFailures;
  private bool $_lastTestFailed;
  private bool $_stop;
  private bool $_stopOnError;
  private bool $_stopOnFailure;
  private bool $_stopOnWarning;
  private bool $_stopOnRisky;
  private bool $_stopOnIncomplete;
  private bool $_stopOnSkipped;
  private float $_time;

  private bool $_beStrictAboutTestsThatDoNotTestAnything;
  private bool $_beStrictAboutOutputDuringTests;
  private bool $_beStrictAboutTodoAnnotatedTests;
  private bool $_beStrictAboutResourceUsageDuringSmallTests;

  private int $_runTests;

  private bool $_enforceTimeLimit;
  private int $_timeoutForSmallTests;
  private int $_timeoutForMediumTests;
  private int $_timeoutForLargeTests;
  private Map<string, Map<string, mixed>> $_passed;
  private ?TestInterface $_topTestSuite;
  private ?CodeCoverage $_codeCoverage;
  private bool $_convertErrorsToExceptions;
  private bool $_errorHandlerSet;
  private mixed $_errorHandlerPrevious;

  public function __construct() {
    $this->_listeners = new Listeners();
    $this->_testFailures = new TestFailures();
    $this->_lastTestFailed = false;
    $this->_stop = false;
    $this->_stopOnError = false;
    $this->_stopOnFailure = false;
    $this->_stopOnWarning = false;
    $this->_stopOnRisky = false;
    $this->_stopOnIncomplete = false;
    $this->_stopOnSkipped = false;
    $this->_time = 0.0;
    $this->_runTests = 0;
    $this->_enforceTimeLimit = false;
    $this->_timeoutForSmallTests = 1;
    $this->_timeoutForMediumTests = 10;
    $this->_timeoutForLargeTests = 60;
    $this->_beStrictAboutTestsThatDoNotTestAnything = false;
    $this->_beStrictAboutOutputDuringTests = false;
    $this->_beStrictAboutTodoAnnotatedTests = false;
    $this->_beStrictAboutResourceUsageDuringSmallTests = false;
    $this->_passed = Map {};
    $this->_topTestSuite = null;
    $this->_codeCoverage = null;
    $this->_convertErrorsToExceptions = true;
    $this->_errorHandlerSet = false;
    $this->_errorHandlerPrevious = null;
  }

  public function listeners(): Listeners {
    return $this->_listeners;
  }

  public function addListener(TestListenerInterface $listener): void {
    $this->_listeners->add($listener);
  }

  public function removeListener(TestListenerInterface $listener): void {
    $this->_listeners->remove($listener);
  }

  public function flushListeners(): void {
    $this->_listeners->flush();
  }

  /**
   * Returns whether the entire test was successful or not.
   *
   * @return bool
   */
  final public function wasSuccessful(): bool {
    return $this->_testFailures->wasSuccessful();
  }

  final public function successfulCount(): int {
    $passed = 0;
    foreach ($this->_passed as $name => $result) {
      if ($name != WarningTestCase::class.'::Warning') {
        $passed++;
      }
    }
    return $passed;
  }

  /**
   * Returns an Enumeration for the warnings.
   *
   * @return array
   *
   * @since Method available since Release 5.1.0
   */
  final public function warnings(): Vector<TestFailure> {
    return $this->_testFailures->warnings();
  }

  /**
   * Gets the number of detected warnings.
   *
   * @return int
   *
   * @since Method available since Release 5.1.0
   */
  final public function warningCount(): int {
    return $this->_testFailures->warningCount();
  }

  /**
   * Returns an Enumeration for the failures.
   *
   * @return array
   */
  final public function failures(): Vector<TestFailure> {
    return $this->_testFailures->failures();
  }

  /**
   * Gets the number of detected failures.
   *
   * @return int
   */
  final public function failureCount(): int {
    return $this->_testFailures->failureCount();
  }

  /**
   * Returns an Enumeration for the errors.
   *
   * @return array
   */
  final public function errors(): Vector<TestFailure> {
    return $this->_testFailures->errors();
  }

  /**
   * Gets the number of detected errors.
   *
   * @return int
   */
  final public function errorCount(): int {
    return $this->_testFailures->errorCount();
  }

  /**
   * Returns an Enumeration for the skipped tests.
   *
   * @return array
   *
   * @since Method available since Release 3.0.0
   */
  final public function skipped(): Vector<TestFailure> {
    return $this->_testFailures->skipped();
  }

  /**
   * Gets the number of skipped tests.
   *
   * @return int
   *
   * @since Method available since Release 3.0.0
   */
  final public function skippedCount(): int {
    return $this->_testFailures->skippedCount();
  }

  /**
   * Returns true if no test has been skipped.
   *
   * @return bool
   *
   * @since Method available since Release 3.0.0
   */
  final public function noneSkipped(): bool {
    return $this->skippedCount() == 0;
  }

  /**
   * Returns an Enumeration for the incomplete tests.
   *
   * @return array
   */
  final public function notImplemented(): Vector<TestFailure> {
    return $this->_testFailures->notImplemented();
  }

  /**
   * Gets the number of incomplete tests.
   *
   * @return int
   */
  final public function notImplementedCount(): int {
    return $this->_testFailures->notImplementedCount();
  }

  /**
   * Returns true if no incomplete test occurred.
   *
   * @return bool
   */
  final public function allCompletelyImplemented(): bool {
    return $this->notImplementedCount() == 0;
  }

  /**
   * Returns an Enumeration for the risky tests.
   *
   * @return array
   *
   * @since Method available since Release 4.0.0
   */
  final public function risky(): Vector<TestFailure> {
    return $this->_testFailures->risky();
  }

  /**
   * Gets the number of risky tests.
   *
   * @return int
   *
   * @since Method available since Release 4.0.0
   */
  final public function riskyCount(): int {
    return $this->_testFailures->riskyCount();
  }

  /**
   * Returns true if no risky test occurred.
   *
   * @return bool
   *
   * @since Method available since Release 4.0.0
   */
  final public function allHarmless(): bool {
    return $this->riskyCount() == 0;
  }

  /**
   * Adds an error to the list of errors.
   *
   * @param TestInterface $test
   * @param Throwable              $t
   * @param float                  $time
   */
  final public function addError(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {
    $this->_testFailures->handleTestFailures($this, $test, $e, $time, false);
  }

  /**
   * Adds a warning to the list of warnings.
   * The passed in exception caused the warning.
   *
   * @param TestInterface    $test
   * @param Exception $e
   * @param float                     $time
   *
   * @since Method available since Release 5.1.0
   */
  final public function addWarning(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {
    $this->_testFailures->handleWarningTestFailure($this, $test, $e, $time);
  }

  /**
   * Adds a failure to the list of failures.
   * The passed in exception caused the failure.
   *
   * @param TestInterface                 $test
   * @param Exception $e
   * @param float                                  $time
   */
  final public function addFailure(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {
    // if ( preg_match('/Requirements/', get_class($test))) {
    // var_dump($test->getName());
    // var_dump(get_class($test));
    // var_dump($e);
    // exit();
    // }
    $this->_testFailures->handleTestFailures($this, $test, $e, $time, true);
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
  final public function stopOnSkipped(bool $flag): void {
    $this->_stopOnSkipped = $flag;
  }

  final public function getStopOnSkipped(): bool {
    return $this->_stopOnSkipped;
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
  final public function stopOnIncomplete(bool $flag): void {
    $this->_stopOnIncomplete = $flag;
  }

  final public function getStopOnIncomplete(): bool {
    return $this->_stopOnIncomplete;
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
  final public function stopOnRisky(bool $flag): void {
    $this->_stopOnRisky = $flag;
  }

  final public function getStopOnRisky(): bool {
    return $this->_stopOnRisky;
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
  final public function stopOnWarning(bool $flag): void {
    $this->_stopOnWarning = $flag;
  }

  final public function getStopOnWarning(): bool {
    return $this->_stopOnWarning;
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
  final public function stopOnFailure(bool $flag): void {
    $this->_stopOnFailure = $flag;
  }

  final public function getStopOnFailure(): bool {
    return $this->_stopOnFailure;
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
  final public function stopOnError(bool $flag): void {
    $this->_stopOnError = $flag;
  }

  final public function getStopOnError(): bool {
    return $this->_stopOnError;
  }

  /**
   * Marks that the test run should stop.
   */
  final public function stop(): void {
    $this->_stop = true;
  }

  /**
   * Checks whether the test run should stop.
   *
   * @return bool
   */
  final public function shouldStop(): bool {
    return $this->_stop;
  }

  final public function setLastTestFailed(bool $lastTestFailed): void {
    $this->_lastTestFailed = $lastTestFailed;
  }

  final public function getLastTestFailed(): bool {
    return $this->_lastTestFailed;
  }

  /**
   * Returns the time spent running the tests.
   *
   * @return float
   */
  final public function time(): float {
    return $this->_time;
  }

  final public function addToTime(float $t): bool {
    $this->_time += $t;
    return true;
  }

  /**
   * @param bool $flag
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since Method available since Release 4.2.0
   */
  final public function beStrictAboutTodoAnnotatedTests(bool $flag): void {
    $this->_beStrictAboutTodoAnnotatedTests = $flag;
  }

  /**
   * @return bool
   *
   * @since Method available since Release 4.2.0
   */
  final public function isStrictAboutTodoAnnotatedTests(): bool {
    return $this->_beStrictAboutTodoAnnotatedTests;
  }

  /**
   * @param bool $flag
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since Method available since Release 5.0.0
   */
  final public function beStrictAboutResourceUsageDuringSmallTests(
    bool $flag,
  ): void {
    $this->_beStrictAboutResourceUsageDuringSmallTests = $flag;
  }

  /**
   * @return bool
   *
   * @since Method available since Release 5.0.0
   */
  final public function isStrictAboutResourceUsageDuringSmallTests(): bool {
    return $this->_beStrictAboutResourceUsageDuringSmallTests;
  }

  /**
   * @param bool $flag
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since Method available since Release 4.0.0
   */
  final public function beStrictAboutOutputDuringTests(bool $flag): void {
    $this->_beStrictAboutOutputDuringTests = $flag;
  }

  /**
   * @return bool
   *
   * @since Method available since Release 4.0.0
   */
  final public function isStrictAboutOutputDuringTests(): bool {
    return $this->_beStrictAboutOutputDuringTests;
  }

  /**
   * @param bool $flag
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since Method available since Release 4.0.0
   */
  final public function beStrictAboutTestsThatDoNotTestAnything(
    bool $flag,
  ): void {
    $this->_beStrictAboutTestsThatDoNotTestAnything = $flag;
  }

  /**
   * @return bool
   *
   * @since Method available since Release 4.0.0
   */
  final public function isStrictAboutTestsThatDoNotTestAnything(): bool {
    return $this->_beStrictAboutTestsThatDoNotTestAnything;
  }

  /**
   * Gets the number of run tests.
   *
   * @return int
   */
  final public function count(): int {
    return $this->_runTests;
  }

  final public function incrementRunTest(int $amt = 0): bool {
    $this->_runTests += $amt;
    return true;
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
  public function setTimeoutForSmallTests(int $timeout): void {
    $this->_timeoutForSmallTests = $timeout;
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
  final public function setTimeoutForMediumTests(int $timeout): void {
    $this->_timeoutForMediumTests = $timeout;
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
  final public function setTimeoutForLargeTests(int $timeout): void {
    $this->_timeoutForLargeTests = $timeout;
  }

  /**
   * Returns the set timeout for large tests.
   *
   * @return int
   */
  final public function getTimeoutForLargeTests(): int {
    return $this->_timeoutForLargeTests;
  }

  /**
   * @param bool $flag
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since Method available since Release 5.0.0
   */
  public function enforceTimeLimit(bool $flag): void {
    $this->_enforceTimeLimit = $flag;
  }

  /**
   * @return bool
   *
   * @since Method available since Release 5.0.0
   */
  public function enforcesTimeLimit(): bool {
    return $this->_enforceTimeLimit;
  }

  /**
   * Informs the result that a testsuite will be started.
   *
   * @param TestSuite $suite
   *
   * @since Method available since Release 2.2.0
   */
  final public function startTestSuite(TestInterface $suite): void {

    $this->listeners()->startTestSuite($suite);

    // if ($this->_topTestSuite === null && $suite instanceof TestSuite) {
    $this->_topTestSuite = $suite;
    //}

  }

  /**
   * Informs the result that a testsuite was completed.
   *
   * @param TestSuite $suite
   *
   * @since Method available since Release 2.2.0
   */
  final public function endTestSuite(TestInterface $suite): void {
    $this->listeners()->endTestSuite($suite);
  }

  /**
   * Informs the result that a test will be started.
   *
   * @param TestInterface $test
   */
  final public function startTest(TestInterface $test): void {

    $this->setLastTestFailed(false);
    $this->incrementRunTest($test->count());
    $this->listeners()->startTest($test);

  }

  /**
   * Informs the result that a test was completed.
   *
   * @param TestInterface $test
   * @param float                  $time
   */
  final public function endTest(TestInterface $test, float $time): void {
    $this->listeners()->endTest($test, $time);

    if (!$this->getLastTestFailed() && $test instanceof TestCase) {
      $class = $test->getClass();
      $key = $class.'::'.$test->getName();

      $data = Map {
        'result' => $test->getResult(),
        'size' => $test->getSize(),
      };

      $this->_passed->set($key, $data);

      $this->addToTime($time);
    }
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
  final public function convertErrorsToExceptions(bool $flag): void {
    $this->_convertErrorsToExceptions = $flag;
  }

  /**
   * Returns the error-to-exception conversion setting.
   *
   * @return bool
   *
   * @since Method available since Release 3.4.0
   */
  final public function getConvertErrorsToExceptions(): bool {
    return $this->_convertErrorsToExceptions;
  }
  /**
   * Returns the code coverage object.
   *
   * @return CodeCoverage
   *
   * @since Method available since Release 3.5.0
   */
  final public function getCodeCoverage(): ?CodeCoverage {
    return $this->_codeCoverage;
  }

  /**
   * Sets the code coverage object.
   *
   * @param CodeCoverage $codeCoverage
   *
   * @since Method available since Release 3.6.0
   */
  final public function setCodeCoverage(CodeCoverage $codeCoverage): void {
    $this->_codeCoverage = $codeCoverage;
  }

  /**
   * Returns the names of the tests that have passed.
   *
   * @return array
   *
   * @since Method available since Release 3.4.0
   */
  final public function passed(): Map<string, Map<string, mixed>> {
    return $this->_passed;
  }

  /**
   * Returns the (top) test suite.
   *
   * @return TestSuite
   *
   * @since Method available since Release 3.0.0
   */
  public function topTestSuite(): ?TestInterface {
    return $this->_topTestSuite;
  }

  /**
   * Returns whether code coverage information should be collected.
   *
   * @return bool If code coverage should be collected
   *
   * @since Method available since Release 3.2.0
   */
  public function getCollectCodeCoverageInformation(): bool {
    return $this->_codeCoverage !== null;
  }

  public function run_handleErrorHandler(): void {
    if ($this->_convertErrorsToExceptions) {
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
  public function run(TestInterface $test): void {

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

    $this->run_handleErrorHandler();

    $codeCoverage = $this->getCodeCoverage();

    $collectCodeCoverage =
      $codeCoverage !== null &&
      !$test instanceof PHPUnit_Framework_WarningTestCase;

    if ($collectCodeCoverage && $codeCoverage instanceof CodeCoverage) {
      $codeCoverage->start($test);
    }

    $this->startTest($test);

    $monitorFunctions =
      $this->isStrictAboutResourceUsageDuringSmallTests() &&
      !$test instanceof PHPUnit_Framework_WarningTestCase &&
      $test->getSize() == Size::SMALL &&
      function_exists('xdebug_start_function_monitor');

    if ($monitorFunctions) {
      XDebug::startMonitoringFunctions(ResourceOperations::getFunctions());
    }

    try {

      $test->runBare($this);

      // @TODO: Cleanup this block.
      // JEO: I am pretty sure mock objects don't work anymore therefor this trap
      // is also dead.
      // } catch (PHPUnit_Framework_MockObject_Exception $e) {
      //   $e = new WarningException($e->getMessage());
      //   $warning = true;

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
      // @TODO: cleanup this catch as it doesn't apply anymore.
      // JEO: you cannot bind against throwable in hack strict.
      // } catch (Throwable $e) {
      //   $e = new ExceptionWrapper($e);
      //   $error = true;
    } catch (Exception $e) {
      // var_dump('TestResult::Exception trap');
      // var_dump(get_class($e));
      // var_dump($e->getMessage());
      // // var_dump(debug_backtrace(2));
      // var_dump($e->getTrace());
      // exit();
      $e = new ExceptionWrapper($e);
      $error = true;
    }

    $time = $test->getBareElapsed();

    // $test->addToAssertionCount(PHPUnit_Framework_Assert::getCount());
    $test->addToAssertionCount($test->getCount());

    if ($monitorFunctions) {
      $functions = XDebug::getMonitoredFunctions();

      XDebug::stopMonitoringFunctions();

      foreach ($functions as $function) {

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

    if ($this->isStrictAboutTestsThatDoNotTestAnything() &&
        $test->getNumAssertions() == 0) {
      $risky = true;
    }

    if ($collectCodeCoverage) {

      try {

        if ($codeCoverage instanceof CodeCoverage) {
          $codeCoverage->stop();
          // @TODO: This function signature was here in stop(...)
          //$codeCoverage->stop($append, $linesToBeCovered, $linesToBeUsed);
        }

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
      // @TODO: See if this is still true, while the code is un-typed it
      //        is true.
      // --
      // Not 100% certain this cannot happen, but as we tighten up the types
      // throughout the code, I wanted to leave this trap in.
      // --
      error_log(
        'JEO WARNING - NON-Exception based error thrown e='.gettype($e),
      );
    } else if ($error === true && $skipped == true) {
      // skipped is a noop atm.
    } else if ($error === true && $e instanceof Exception) {
      $this->addError($test, $e, $time);
    } else if ($failure === true && $e instanceof Exception) {
      $this->addFailure($test, $e, $time);
    } else if ($warning === true && $e instanceof Exception) {
      $this->addWarning($test, $e, $time);
    } else if ($this->isStrictAboutTestsThatDoNotTestAnything() &&
               $test->getNumAssertions() == 0 &&
               !$test instanceof IncompleteTestCase) {
      $this->addFailure(
        $test,
        new RiskyException('This test did not perform any assertions'),
        $time,
      );
    } else if ($this->isStrictAboutOutputDuringTests() &&
               $test->hasOutput()) {

      $actualOutput = $test->getActualOutput();

      if (strlen($actualOutput) > 255) {
        $actualOutput = substr($actualOutput, 0, 255).'<...shortened...>';
      }

      $this->addFailure(
        $test,
        new PHPUnit_Framework_OutputError(
          sprintf('This test printed output: %s', $actualOutput).
          ' startedAt file='.
          $test->getOutputFile().
          '::'.
          $test->getOutputLine(),
        ),
        $time,
      );

    } else if ($this->isStrictAboutTodoAnnotatedTests() &&
               $test instanceof TestCase) {

      $annotations = $test->getAnnotationsForKey('method', 'todo');

      if ($annotations->count() > 0) {
        $this->addFailure(
          $test,
          new RiskyException('Test method is annotated with @todo'),
          $time,
        );
      }

    }

    $this->endTest($test, $time);

  }

}
