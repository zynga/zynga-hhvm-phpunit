<?hh // strict

namespace PHPUnit\Framework;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use PHPUnit\Exceptions\AssertionFailedError;
use PHPUnit\Exceptions\Error;
use PHPUnit\Exceptions\Exception as PHPUnit_Exceptions_Exception;
use PHPUnit\Exceptions\ExpectationFailedException;
use PHPUnit\Exceptions\ExceptionWrapper;
use PHPUnit\Interfaces\ErrorExceptionInterface;
use PHPUnit\Interfaces\SelfDescribingInterface;
use PHPUnit\Interfaces\TestInterface;
use SebastianBergmann\Comparator\ComparisonFailure;

use \Exception;

/**
 * A TestFailure collects a failed test together with the caught exception.
 *
 * @since Class available since Release 2.0.0
 */
class TestFailure {

    /**
     * @var string
     */
    private string $testName;

    /**
     * @var TestInterface|null
     */
    protected TestInterface $failedTest;

    /**
     * @var Exception
     */
    protected Exception $thrownException;

    /**
     * Constructs a TestFailure with the given test and exception.
     *
     * @param PHPUnit_Framework_Test $failedTest
     * @param Throwable              $t
     */
    public function __construct(TestInterface $failedTest, Exception $t) {
        $this->testName = get_class($failedTest);
        $this->failedTest = $failedTest;
        $this->thrownException = $t;
    }

    /**
     * Returns a short description of the failure.
     *
     * @return string
     */
    public function toString(): string {
      return sprintf(
        '%s: %s',
        $this->testName,
        $this->thrownException->getMessage()
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
    public static function exceptionToString(Exception $e): string {

      if ($e instanceof SelfDescribingInterface) {

        $buffer = $e->toString();

        if ($e instanceof ExpectationFailedException && $e->getComparisonFailure()) {
          $diff = '';
          $comparisonFailure = $e->getComparisonFailure();
          if ( $comparisonFailure instanceof ComparisonFailure ) {
            $buffer .= $comparisonFailure->getDiff();
          }
        }

        if ($buffer != '') {
          $buffer = trim($buffer) . "\n";
        }
      } elseif ($e instanceof ErrorExceptionInterface && $e instanceof Exception) {
        $buffer = $e->getMessage() . "\n";
      } elseif ($e instanceof ExceptionWrapper) {
        $buffer = $e->getClassname() . ': ' . $e->getMessage() . "\n";
      } else {
        $buffer = get_class($e) . ': ' . $e->getMessage() . "\n";
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
      return $this->testName;
    }

    /**
     * Returns the failing test.
     *
     * Note: The test object is not set when the test is executed in process
     * isolation.
     *
     * @see PHPUnit_Exceptions_Exception
     *
     * @return PHPUnit_Framework_Test|null
     */
    public function failedTest(): mixed {
      return $this->failedTest;
    }

    /**
     * Gets the thrown exception.
     *
     * @return Exception
     */
    public function thrownException(): Exception {
      return $this->thrownException;
    }

    /**
     * Returns the exception's message.
     *
     * @return string
     */
    public function exceptionMessage(): string
    {
      return $this->thrownException()->getMessage();
    }

    /**
     * Returns true if the thrown exception
     * is of type AssertionFailedError.
     *
     * @return bool
     */
    public function isFailure(): bool {
      return ($this->thrownException() instanceof AssertionFailedError);
    }

}
