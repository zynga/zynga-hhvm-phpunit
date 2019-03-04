<?hh

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use SebastianBergmann\PHPUnit\Exceptions\TestError\IncompleteException;
use Zynga\PHPUnit\V2\Interfaces\TestInterface;
use Zynga\PHPUnit\V2\TestCase;

/**
 * A TestFailure collects a failed test together with the caught exception.
 *
 * @since Class available since Release 2.0.0
 */
class PHPUnit_Framework_TestFailure {
  /**
   * @var string
   */
  private $testName;

  /**
   * @var TestInterface|null
   */
  protected $failedTest;

  /**
   * @var Exception
   */
  protected $thrownException;

  /**
   * Constructs a TestFailure with the given test and exception.
   *
   * @param TestInterface $failedTest
   * @param Throwable              $t
   */
  public function __construct(TestInterface $failedTest, Exception $t) {
    if ($failedTest instanceof PHPUnit_Framework_SelfDescribing) {
      $this->testName = $failedTest->toString();
    } else if ($failedTest instanceof TestCase) {
      $this->testName = $failedTest->toString();
    } else {
      $this->testName = get_class($failedTest);
    }

    if (!$failedTest instanceof TestCase) {
      $this->failedTest = $failedTest;

    }

    $this->thrownException = $t;
  }

  /**
   * Returns a short description of the failure.
   *
   * @return string
   */
  public function toString() {
    return sprintf(
      '%s: %s',
      $this->testName,
      $this->thrownException->getMessage(),
    );
  }

  /**
   * Returns a description for the thrown exception.
   *
   * @return string
   *
   * @since Method available since Release 3.4.0
   */
  public function getExceptionAsString() {
    return self::exceptionToString($this->thrownException);
  }

  /**
   * Returns a description for an exception.
   *
   * @param Exception $e
   *
   * @return string
   *
   * @since Method available since Release 3.2.0
   */
  public static function exceptionToString(Exception $e) {
    if ($e instanceof PHPUnit_Framework_SelfDescribing) {
      $buffer = $e->toString();

      // if ($e instanceof PHPUnit_Framework_ExpectationFailedException) {
      //   $buffer = $buffer.$e->getComparisonFailure()->getDiff();
      // }

      if (!empty($buffer)) {
        $buffer = trim($buffer)."\n";
      }
    } else if ($e instanceof PHPUnit_Framework_Error) {
      $buffer = $e->getMessage()."\n";
    } else {
      $buffer = get_class($e).': '.$e->getMessage()."\n";
    }

    return $buffer;
  }

  /**
   * Returns the name of the failing test (including data set, if any).
   *
   * @return string
   *
   * @since Method available since Release 4.3.0
   */
  public function getTestName() {
    return $this->testName;
  }

  /**
   * Returns the failing test.
   *
   * Note: The test object is not set when the test is executed in process
   * isolation.
   *
   * @see PHPUnit_Framework_Exception
   *
   * @return TestInterface|null
   */
  public function failedTest() {
    return $this->failedTest;
  }

  /**
   * Gets the thrown exception.
   *
   * @return Exception
   */
  public function thrownException() {
    return $this->thrownException;
  }

  /**
   * Returns the exception's message.
   *
   * @return string
   */
  public function exceptionMessage() {
    return $this->thrownException()->getMessage();
  }

  /**
   * Returns true if the thrown exception
   * is of type AssertionFailedError.
   *
   * @return bool
   */
  public function isFailure() {
    return ($this->thrownException() instanceof AssertionFailedException);
  }
}
