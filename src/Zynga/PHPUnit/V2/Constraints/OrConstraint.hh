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
 * Logical OR.
 *
 * @since Class available since Release 3.0.0
 */
class OrConstraint extends Base {

  protected Vector<ConstraintInterface> $constraints = Vector {};

  public function setExpected(mixed $expected): bool {
    if ($expected instanceof Vector) {
      return $this->setConstraints($expected);
    }
    return false;
  }

  public function addConstraint(ConstraintInterface $constraint): bool {
    $this->constraints->add($constraint);
    return true;
  }

  private function setConstraints(
    Vector<ConstraintInterface> $constraints,
  ): bool {
    $this->constraints = $constraints;
    return true;
  }

  public function resetExpected(): bool {
    $this->constraints->clear();
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
  public function evaluate(
    mixed $other,
    string $description = '',
    bool $doThrowException = false,
  ): bool {

    $success = false;
    $constraint = null;

    foreach ($this->constraints as $constraint) {
      if ($constraint->evaluate($other, $description, true)) {
        $success = true;
        break;
      }
    }

    if ($doThrowException && !$success) {
      $this->fail($other, $description);
    }

    return $success;

  }

  /**
   * Returns a string representation of the constraint.
   *
   * @return string
   */
  public function toString(): string {
    $text = '';

    foreach ($this->constraints as $key => $constraint) {
      if ($key > 0) {
        $text .= ' or ';
      }

      $text .= $constraint->toString();
    }

    return $text;
  }

  /**
   * Counts the number of constraint elements.
   *
   * @return int
   *
   * @since Method available since Release 3.4.0
   */
  public function count(): int {
    return $this->constraints->count();
  }

}
