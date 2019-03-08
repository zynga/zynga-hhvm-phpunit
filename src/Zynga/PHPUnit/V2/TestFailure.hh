<?hh // strict

namespace Zynga\PHPUnit\V2;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use SebastianBergmann\PHPUnit\Exceptions\AssertionFailedException;
use SebastianBergmann\PHPUnit\Exceptions\TestError\IncompleteException;
use Zynga\PHPUnit\V2\Interfaces\TestInterface;
use Zynga\PHPUnit\V2\TestCase;
use \PHPUnit_Framework_SelfDescribing;
use \PHPUnit_Framework_Error;
use \Exception;

/**
 * A TestFailure collects a failed test together with the caught exception.
 *
 * @since Class available since Release 2.0.0
 */
class TestFailure {
  private string $_testName;
  private ?TestInterface $_failedTest;
  private Exception $_thrownException;

  /**
   * Constructs a TestFailure with the given test and exception.
   *
   * @param TestInterface $failedTest
   * @param Throwable              $t
   */
  public function __construct(TestInterface $failedTest, Exception $t) {

    if (!$failedTest instanceof TestCase) {
      $this->_failedTest = $failedTest;
    }

    if ($failedTest instanceof PHPUnit_Framework_SelfDescribing) {
      $this->_testName = $failedTest->toString();
    } else if ($failedTest instanceof TestCase) {
      $this->_testName = $failedTest->toString();
    } else {
      $this->_testName = get_class($failedTest);
    }

    $this->_thrownException = $t;

  }

  /**
   * Returns a short description of the failure.
   *
   * @return string
   */
  public function toString(): string {
    return sprintf(
      '%s: %s',
      $this->_testName,
      $this->_thrownException->getMessage(),
    );
  }

  /**
   * Returns a description for the thrown exception.
   *
   * @return string
   *
   * @since Method available since Release 3.4.0
   */
  public function getExceptionAsString(): string {
    return self::exceptionToString($this->_thrownException);
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
  public static function exceptionToString(Exception $e): string {
    if ($e instanceof PHPUnit_Framework_SelfDescribing) {
      $buffer = $e->toString();

      // if ($e instanceof PHPUnit_Framework_ExpectationFailedException) {
      //   $buffer = $buffer.$e->getComparisonFailure()->getDiff();
      // }

      if ($buffer != '') {
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
  public function getTestName(): string {
    return $this->_testName;
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
  public function failedTest(): ?TestInterface {
    return $this->_failedTest;
  }

  /**
   * Gets the thrown exception.
   *
   * @return Exception
   */
  public function thrownException(): Exception {
    return $this->_thrownException;
  }

  /**
   * Returns the exception's message.
   *
   * @return string
   */
  public function exceptionMessage(): string {
    return $this->thrownException()->getMessage();
  }

  /**
   * Returns true if the thrown exception
   * is of type AssertionFailedError.
   *
   * @return bool
   */
  public function isFailure(): bool {
    return ($this->thrownException() instanceof AssertionFailedException);
  }
}
