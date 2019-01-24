<?hh // strict

namespace SebastianBergmann\PHPUnit\Constraints;

use SebastianBergmann\PHPUnit\Constraints\Base;
use SebastianBergmann\PHPUnit\Interfaces\ConstraintInterface;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

/**
 * Logical OR.
 *
 * @since Class available since Release 3.0.0
 */
class LogicalOrConstraint extends Base {

  private Vector<ConstraintInterface> $constraints = Vector {};

  public function setExpected(mixed $value): bool {
    return true;
  }

  public function resetExpected(): bool {
    return true;
  }

  public function addConstraint(ConstraintInterface $constraint): bool {
    $this->constraints->add($constraint);
    return true;
  }

  public function clearConstraints(): bool {
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
  public function matches(mixed $other): bool {

    foreach ($this->constraints as $constraint) {
      if ($constraint->matches($other) === true) {
        return true;
      }
    }

    return false;

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
    $count = 0;

    foreach ($this->constraints as $constraint) {
      $count += count($constraint);
    }

    return $count;
  }

}
