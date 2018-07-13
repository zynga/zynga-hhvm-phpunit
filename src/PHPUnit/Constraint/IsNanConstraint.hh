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

/**
 * Constraint that accepts nan.
 *
 * @since      Class available since Release 5.0.0
 */
class IsNanConstraint extends ConstraintBase {

    /**
     * Evaluates the constraint for parameter $other. Returns true if the
     * constraint is met, false otherwise.
     *
     * @param mixed $other Value or object to evaluate.
     *
     * @return bool
     */
    public function matches(mixed $other): bool {
      if ( is_float($other) ) {
        return is_nan($other);
      }
      return false;
    }

    /**
     * Returns a string representation of the constraint.
     *
     * @return string
     */
    public function toString(): string {
      return 'is nan';
    }
}
