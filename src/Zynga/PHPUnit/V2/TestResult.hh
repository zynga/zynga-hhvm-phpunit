<?hh // strict

namespace Zynga\PHPUnit\V2;

use Zynga\PHPUnit\V2\Interfaces\TestInterface;
use Zynga\PHPUnit\V2\Interfaces\TestListenerInterface;
use Zynga\PHPUnit\V2\TestResult\Listeners;
use Zynga\PHPUnit\V2\TestResult\TestFailures;
use \Exception;

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

}
