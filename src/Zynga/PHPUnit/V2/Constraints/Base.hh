<?hh // strict

namespace Zynga\PHPUnit\V2\Constraints;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use SebastianBergmann\Exporter\Exporter;
use SebastianBergmann\Comparator\ComparisonFailure;
use Zynga\PHPUnit\V2\Exceptions\ExpectationFailedException;
use Zynga\PHPUnit\V2\Interfaces\SelfDescribingInterface;
use Zynga\PHPUnit\V2\Interfaces\ConstraintInterface;
use \Countable;

/**
 * Abstract base class for constraints. which are placed upon any value.
 *
 * @since      Interface available since Release 3.0.0
 */
abstract class Base
  implements Countable, SelfDescribingInterface, ConstraintInterface {

  public function __construct() {}

  /**
   * Evaluates the constraint for parameter $other
   *
   * If $returnResult is set to false (the default), an exception is thrown
   * in case of a failure. null is returned otherwise.
   *
   * If $returnResult is true, the result of the evaluation is returned as
   * a boolean value instead: true in case of success, false in case of a
   * failure.
   *
   * @param mixed  $other        Value or object to evaluate.
   * @param string $description  Additional information about the test
   * @param bool   $returnResult Whether to return a result or throw an exception
   *
   * @return mixed
   *
   * @throws PHPUnit_Framework_ExpectationFailedException
   */
  public function evaluate(
    mixed $other,
    string $description = '',
    bool $doThrowException = false,
  ): bool {

    $success = false;

    if ($this->matches($other)) {
      $success = true;
    }

    if ($doThrowException && $success !== true) {
      $this->fail($other, $description);
    }

    return $success;

  }

  /**
   * Evaluates the constraint for parameter $other. Returns true if the
   * constraint is met, false otherwise.
   *
   * This method can be overridden to implement the evaluation algorithm.
   *
   * @param mixed $other Value or object to evaluate.
   *
   * @return bool
   */
  public function matches(mixed $other): bool {
    return false;
  }

  /**
   * Counts the number of constraint elements.
   *
   * @return int
   *
   * @since Method available since Release 3.4.0
   */
  public function count(): int {
    return 1;
  }

  /**
   * Throws an exception for the given compared value and test description
   *
   * @param mixed                                          $other             Evaluated value or object.
   * @param string                                         $description       Additional information about the test
   * @param SebastianBergmann\Comparator\ComparisonFailure $comparisonFailure
   *
   * @throws PHPUnit_Framework_ExpectationFailedException
   */
  protected function fail(
    mixed $other,
    string $description,
    ?ComparisonFailure $comparisonFailure = null,
  ): void {
    $failureDescription =
      sprintf('Failed asserting that %s.', $this->failureDescription($other));

    $additionalFailureDescription =
      $this->additionalFailureDescription($other);

    if ($additionalFailureDescription) {
      $failureDescription .= "\n".$additionalFailureDescription;
    }

    if ($description != '') {
      $failureDescription = $description."\n".$failureDescription;
    }

    throw new ExpectationFailedException(
      $failureDescription,
      $comparisonFailure,
    );

  }

  /**
   * Return additional failure description where needed
   *
   * The function can be overridden to provide additional failure
   * information like a diff
   *
   * @param mixed $other Evaluated value or object.
   *
   * @return string
   */
  protected function additionalFailureDescription(mixed $other): string {
    return '';
  }

  protected function getExporter(): Exporter {
    return new Exporter();
  }

  /**
   * Returns the description of the failure
   *
   * The beginning of failure messages is "Failed asserting that" in most
   * cases. This method should return the second part of that sentence.
   *
   * To provide additional failure information additionalFailureDescription
   * can be used.
   *
   * @param mixed $other Evaluated value or object.
   *
   * @return string
   */
  public function failureDescription(mixed $other): string {
    return $this->getExporter()->export($other).' '.$this->toString();
  }

}
