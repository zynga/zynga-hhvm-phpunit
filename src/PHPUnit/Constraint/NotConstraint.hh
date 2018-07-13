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
use PHPUnit\Constraint\IsEqualConstraint;
use PHPUnit\Exceptions\ExpectationFailedException;
use PHPUnit\Interfaces\ConstraintInterface;

/**
 * Logical NOT.
 *
 * @since Class available since Release 3.0.0
 */
class NotConstraint extends ConstraintBase {

    /**
     * @var PHPUnit_Framework_Constraint
     */
    protected ConstraintInterface $constraint;

    /**
     * @param PHPUnit_Framework_Constraint $constraint
     */
    public function __construct(mixed $constraint) {
      parent::__construct();

      if (!($constraint instanceof ConstraintInterface)) {
        $constraint = new IsEqualConstraint($constraint);
      }

      $this->constraint = $constraint;
    }

    /**
     * @param string $string
     *
     * @return string
     */
    public function negate(string $string): string {
      return str_replace(
        [
          'contains ',
          'exists',
          'has ',
          'is ',
          'are ',
          'matches ',
          'starts with ',
          'ends with ',
          'reference ',
          'not not '
        ],
        [
          'does not contain ',
          'does not exist',
          'does not have ',
          'is not ',
          'are not ',
          'does not match ',
          'starts not with ',
          'ends not with ',
          'don\'t reference ',
          'not '
        ],
          $string
      );
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

      $success = !$this->constraint->evaluate($other, $description, true);

      if ($returnResult) {
        return $success;
      }

      if (!$success) {
        $this->fail($other, $description);
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

      $className = get_class($this->constraint);

      if ( $className == 'AndConstraint' || $className == 'NotConstraint' || $className == 'OrConstraint' ) {
        return 'not( ' . $this->constraint->failureDescription($other) . ' )';
      }

      return $this->negate(
        $this->constraint->failureDescription($other)
      );

    }

    /**
     * Returns a string representation of the constraint.
     *
     * @return string
     */
    public function toString(): string {
      $className = get_class($this->constraint);

      if ( $className == 'AndConstraint' || $className == 'NotConstraint' || $className == 'OrConstraint') {
        return 'not( ' . $this->constraint->toString() . ' )';
      }

      return $this->negate(
        $this->constraint->toString()
      );

    }

    /**
     * Counts the number of constraint elements.
     *
     * @return int
     *
     * @since Method available since Release 3.4.0
     */
    public function count(): int {
      return count($this->constraint);
    }

}
