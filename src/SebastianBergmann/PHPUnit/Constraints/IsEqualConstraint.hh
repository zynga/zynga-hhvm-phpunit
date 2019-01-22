<?hh // strict

namespace SebastianBergmann\PHPUnit\Constraints;

use SebastianBergmann\Comparator\ComparisonFailure;
use SebastianBergmann\Comparator\Factory as ComparatorFactory;
use SebastianBergmann\PHPUnit\Constraints\Base;
use \Exception;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

/**
 * Constraint that checks if one value is equal to another.
 *
 * Equality is checked with PHP's == operator, the operator is explained in
 * detail at {@url http://www.php.net/manual/en/types.comparisons.php}.
 * Two values are equal if they have the same value disregarding type.
 *
 * The expected value is passed in the constructor.
 *
 * @since Class available since Release 3.0.0
 */
class IsEqualConstraint extends Base {
  private mixed $value = null;
  private float $delta = 0.0;
  private int $maxDepth = 10;
  private bool $canonicalize = false;
  private bool $ignoreCase = false;

  public function setExpected(mixed $value): bool {
    $this->value = $value;
    return true;
  }

  public function resetExpected(): bool {
    $this->value = null;
    return true;
  }

  public function setDelta(float $value): bool {
    $this->delta = $value;
    return true;
  }

  public function setMaxDepth(int $value): bool {
    $this->maxDepth = $value;
    return true;
  }

  public function setCanonicalize(bool $value): bool {
    $this->canonicalize = $value;
    return true;
  }

  public function setIgnoreCase(bool $value): bool {
    $this->ignoreCase = $value;
    return true;
  }

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
  public function matches(mixed $other): bool {

    // If $this->value and $other are identical, they are also equal.
    // This is the most common path and will allow us to skip
    // initialization of all the comparators.
    if ($this->value === $other) {
      return true;
    }

    $comparatorFactory = ComparatorFactory::getInstance();

    try {

      $comparator =
        $comparatorFactory->getComparatorFor($this->value, $other);

      $comparator->assertEquals(
        $this->value,
        $other,
        $this->delta,
        $this->canonicalize,
        $this->ignoreCase,
      );

      return true;

    } catch (ComparisonFailure $f) {

      return false;

    }

  }

  /**
   * Returns a string representation of the constraint.
   *
   * @return string
   */
  public function toString(): string {
    $delta = '';

    if (is_string($this->value)) {
      if (strpos($this->value, "\n") !== false) {
        return 'is equal to <text>';
      } else {
        return sprintf('is equal to <string:%s>', $this->value);
      }
    } else {
      if ($this->delta != 0) {
        $delta = sprintf(' with delta <%F>', $this->delta);
      }

      return sprintf(
        'is equal to %s%s',
        $this->getExporter()->export($this->value),
        $delta,
      );
    }
  }
}
