<?hh // strict

namespace Zynga\PHPUnit\V2\TestResult;

use Zynga\PHPUnit\V2\Exceptions\AssertionFailedException;
use Zynga\PHPUnit\V2\Exceptions\ExpectationFailedException;
use Zynga\PHPUnit\V2\Exceptions\InvalidArgumentException;
use Zynga\PHPUnit\V2\Exceptions\WarningException;
use Zynga\PHPUnit\V2\Exceptions\TestError\IncompleteException;
use Zynga\PHPUnit\V2\Exceptions\TestError\RiskyException;
use Zynga\PHPUnit\V2\Exceptions\TestError\SkippedException;
use Zynga\Framework\Dynamic\V1\DynamicMethodCall;
use Zynga\PHPUnit\V2\Interfaces\TestInterface;
use Zynga\PHPUnit\V2\TestFailure;
use Zynga\PHPUnit\V2\TestResult;
use \PHPUnit_Framework_RiskyTest;
use \Exception;

class TestFailures {

  private Vector<TestFailure> $_errors = Vector {};
  private Vector<TestFailure> $_failures = Vector {};
  private Vector<TestFailure> $_warnings = Vector {};
  private Vector<TestFailure> $_notImplemented = Vector {};
  private Vector<TestFailure> $_risky = Vector {};
  private Vector<TestFailure> $_skipped = Vector {};

  /**
   * Returns whether the entire test was successful or not.
   *
   * @return bool
   */
  final public function wasSuccessful(): bool {
    return
      $this->_errors->count() == 0 &&
      $this->_failures->count() == 0 &&
      $this->_warnings->count() == 0;
  }

  /**
   * Returns an Enumeration for the warnings.
   *
   * @return array
   *
   * @since Method available since Release 5.1.0
   */
  final public function warnings(): Vector<TestFailure> {
    return $this->_warnings;
  }

  /**
   * Gets the number of detected warnings.
   *
   * @return int
   *
   * @since Method available since Release 5.1.0
   */
  final public function warningCount(): int {
    return $this->_warnings->count();
  }

  /**
   * Returns an Enumeration for the failures.
   *
   * @return array
   */
  final public function failures(): Vector<TestFailure> {
    return $this->_failures;
  }

  /**
   * Gets the number of detected failures.
   *
   * @return int
   */
  final public function failureCount(): int {
    return $this->_failures->count();
  }

  /**
   * Returns an Enumeration for the errors.
   *
   * @return array
   */
  final public function errors(): Vector<TestFailure> {
    return $this->_errors;
  }

  /**
   * Gets the number of detected errors.
   *
   * @return int
   */
  final public function errorCount(): int {
    return $this->_errors->count();
  }

  /**
   * Returns an Enumeration for the skipped tests.
   *
   * @return array
   *
   * @since Method available since Release 3.0.0
   */
  final public function skipped(): Vector<TestFailure> {
    return $this->_skipped;
  }

  /**
   * Gets the number of skipped tests.
   *
   * @return int
   *
   * @since Method available since Release 3.0.0
   */
  final public function skippedCount(): int {
    return $this->_skipped->count();
  }

  /**
   * Returns an Enumeration for the incomplete tests.
   *
   * @return array
   */
  final public function notImplemented(): Vector<TestFailure> {
    return $this->_notImplemented;
  }

  /**
   * Gets the number of incomplete tests.
   *
   * @return int
   */
  final public function notImplementedCount(): int {
    return $this->_notImplemented->count();
  }

  /**
   * Returns an Enumeration for the risky tests.
   *
   * @return array
   *
   * @since Method available since Release 4.0.0
   */
  final public function risky(): Vector<TestFailure> {
    return $this->_risky;
  }

  /**
   * Gets the number of risky tests.
   *
   * @return int
   *
   * @since Method available since Release 4.0.0
   */
  final public function riskyCount(): int {
    return $this->_risky->count();
  }

  final public function handleTestFailure(
    TestResult $tr,
    Vector<TestFailure> $trackingArray,
    bool $doStop,
    string $listenerFunction,
    TestInterface $test,
    Exception $t,
    float $time,
  ): void {

    $failure = new TestFailure($test, $t);

    $trackingArray->add($failure);

    DynamicMethodCall::callMethodOnObject(
      $tr->listeners(),
      $listenerFunction,
      Vector {$test, $t, $time},
    );

    if ($doStop == true) {
      $tr->stop();
    }

  }

  final public function handleRiskyTestFailure(
    TestResult $tr,
    TestInterface $test,
    Exception $t,
    float $time,
  ): void {
    $this->handleTestFailure(
      $tr,
      $this->_risky,
      $tr->getStopOnRisky(),
      'addRiskyTest',
      $test,
      $t,
      $time,
    );
  }

  final public function handleIncompleteTestFailure(
    TestResult $tr,
    TestInterface $test,
    Exception $t,
    float $time,
  ): void {
    $this->handleTestFailure(
      $tr,
      $this->_notImplemented,
      $tr->getStopOnIncomplete(),
      'addIncompleteTest',
      $test,
      $t,
      $time,
    );
  }

  final public function handleWarningTestFailure(
    TestResult $tr,
    TestInterface $test,
    Exception $t,
    float $time,
  ): void {
    $this->handleTestFailure(
      $tr,
      $this->_warnings,
      $tr->getStopOnWarning(),
      'addWarning',
      $test,
      $t,
      $time,
    );
  }

  final public function handleSkippedTestFailure(
    TestResult $tr,
    TestInterface $test,
    Exception $t,
    float $time,
  ): void {
    $this->handleTestFailure(
      $tr,
      $this->_skipped,
      $tr->getStopOnSkipped(),
      'addSkippedTest',
      $test,
      $t,
      $time,
    );
  }

  // --
  // JEO: Somtimes the typechecker fixates on the last usage of instanceof
  // and in this case we don't want to refine the type.
  //
  // TOOD: remove the RiskyTest implementation.
  //
  // --
  private function isRiskyTest(Exception $e): bool {
    if ($e instanceof PHPUnit_Framework_RiskyTest) {
      return true;
    }
    return false;
  }

  final public function handleTestFailures(
    TestResult $tr,
    TestInterface $test,
    Exception $e,
    float $time,
    bool $isFailure,
  ): void {

    if ($this->isRiskyTest($e) === true) {
      $this->handleRiskyTestFailure($tr, $test, $e, $time);
    } else if ($e instanceof IncompleteException) {
      $this->handleIncompleteTestFailure($tr, $test, $e, $time);
    } else if ($e instanceof SkippedException) {
      $this->handleSkippedTestFailure($tr, $test, $e, $time);
    } else if ($e instanceof WarningException) {
      $this->handleWarningTestFailure($tr, $test, $e, $time);
    } else if ($isFailure == true) {
      $this->handleTestFailure(
        $tr,
        $this->_failures,
        ($tr->getStopOnError() || $tr->getStopOnFailure()),
        'addFailure',
        $test,
        $e,
        $time,
      );
    } else {
      $this->handleTestFailure(
        $tr,
        $this->_errors,
        ($tr->getStopOnError() || $tr->getStopOnFailure()),
        'addError',
        $test,
        $e,
        $time,
      );
    }

    $tr->setLastTestFailed(true);

    $tr->addToTime($time);

  }

}
