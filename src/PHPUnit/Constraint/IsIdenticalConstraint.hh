<?hh // strict

namespace PHPUnit\Constraint;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use PHPUnit\Constraint\Base as ConstraintBase;
use PHPUnit\Exceptions\ExpectationFailedException;
use SebastianBergmann\Comparator\ComparisonFailure;

/**
 * Constraint that asserts that one value is identical to another.
 *
 * Identical check is performed with PHP's === operator, the operator is
 * explained in detail at
 * {@url http://www.php.net/manual/en/types.comparisons.php}.
 * Two values are identical if they have the same value and are of the same
 * type.
 *
 * The expected value is passed in the constructor.
 *
 * @since Class available since Release 3.0.0
 */
class IsIdenticalConstraint extends ConstraintBase {

    /**
     * @var float
     */
    const EPSILON = 0.0000000001;

    /**
     * @var mixed
     */
    protected mixed $value;

    /**
     * @param mixed $value
     */
    public function __construct(mixed $value) {

      parent::__construct();
      $this->value = $value;

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
     * @throws ExpectationFailedException
     */
    public function evaluate(mixed $other, string $description = '', bool $returnResult = false): mixed {

      $value = $this->value;

      if (is_float($value) && is_float($other) &&
          !is_infinite($value) && !is_infinite($other)
      ) {
        $success = abs($value - $other) < self::EPSILON;
      } else {
        $success = $value === $other;
      }

      if ($returnResult) {
        return $success;
      }

      if (!$success) {
        $f = null;

        // if both values are strings, make sure a diff is generated
        if (is_string($value) && is_string($other)) {
          $f = new ComparisonFailure(
            $this->value,
            $other,
            $this->value,
            $other
          );
        }

        $this->fail($other, $description, $f);
      }

    }

    /**
     * Returns the description of the failure
     *
     * The beginning of failure messages is "Failed asserting that" in most
     * cases. This method should return the second part of that sentence.
     *
     * @param mixed $other Evaluated value or object.
     *
     * @return string
     */
    public function failureDescription(mixed $other): string {

      if (is_object($this->value) && is_object($other)) {
        return 'two variables reference the same object';
      }

      if (is_string($this->value) && is_string($other)) {
        return 'two strings are identical';
      }

      return parent::failureDescription($other);

    }

    /**
     * Returns a string representation of the constraint.
     *
     * @return string
     */
    public function toString(): string {

      if (is_object($this->value)) {
        return 'is identical to an object of class "' .
          get_class($this->value) . '"';
      } else {
        return 'is identical to ' .
          $this->exporter->export($this->value);
      }

    }
}
