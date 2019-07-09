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

use Zynga\PHPUnit\V2\Constraints\Base;
use Zynga\PHPUnit\V2\Interfaces\ConstraintInterface;
use \Exception;

/**
 * Logical NOT.
 *
 * @since Class available since Release 3.0.0
 */
class NotConstraint extends Base {

  protected ?ConstraintInterface $constraint;

  public function setExpected(mixed $expected): bool {
    if ($expected instanceof ConstraintInterface) {
      $this->constraint = $expected;
    }
    return false;
  }

  public function resetExpected(): bool {
    $this->constraint = null;
    return true;
  }

  /**
   * @param string $string
   *
   * @return string
   */
  public static function negate(string $string): string {
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
        'not not ',
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
        'not ',
      ],
      $string,
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
   * @throws PHPUnit_Framework_ExpectationFailedException
   */
  public function evaluate(
    mixed $other,
    string $description = '',
    bool $doThrowException = false,
  ): bool {

    if (!$this->constraint instanceof ConstraintInterface) {
      return false;
    }

    $success = !$this->constraint->evaluate($other, $description, false);

    if ($doThrowException && !$success) {
      $this->fail($other, $description);
    }

    return $success;

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
    if (!$this->constraint instanceof ConstraintInterface) {
      return 'invalid-expected-constraint';
    }
    if ($this->constraint instanceof AndConstraint) {
      return 'not( '.$this->constraint->failureDescription($other).' )';
    } else if ($this->constraint instanceof NotConstraint) {
      return 'not( '.$this->constraint->failureDescription($other).' )';
    } else if ($this->constraint instanceof OrConstraint) {
      return 'not( '.$this->constraint->failureDescription($other).' )';
    }
    return self::negate($this->constraint->failureDescription($other));
  }

  /**
   * Returns a string representation of the constraint.
   *
   * @return string
   */
  public function toString(): string {
    if (!$this->constraint instanceof ConstraintInterface) {
      return 'invalid-expected-constraint';
    }
    if ($this->constraint instanceof AndConstraint) {
      return 'not( '.$this->constraint->toString().' )';
    } else if ($this->constraint instanceof NotConstraint) {
      return 'not( '.$this->constraint->toString().' )';
    } else if ($this->constraint instanceof OrConstraint) {
      return 'not( '.$this->constraint->toString().' )';
    }
    return self::negate($this->constraint->toString());
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

}
