<?hh // strict

namespace SebastianBergmann\PHPUnit\Constraints;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use SebastianBergmann\PHPUnit\Constraints\Base;

/**
 * Constraint that accepts finite.
 *
 * @since      Class available since Release 5.0.0
 */
class IsFiniteConstraint extends Base {

  public function setExpected(mixed $value): bool {
    return true;
  }

  public function resetExpected(): bool {
    return true;
  }

  /**
   * Evaluates the constraint for parameter $other. Returns true if the
   * constraint is met, false otherwise.
   *
   * @param mixed $other Value or object to evaluate.
   *
   * @return bool
   */
  public function matches(mixed $other): bool {
    return is_finite($other);
  }

  /**
   * Returns a string representation of the constraint.
   *
   * @return string
   */
  public function toString(): string {
    return 'is finite';
  }

}
