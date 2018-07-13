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
 * Constraint that asserts that the value it is evaluated for is greater
 * than a given value.
 *
 * @since Class available since Release 3.0.0
 */
class GreaterThanConstraint extends ConstraintBase {

    /**
     * @var numeric
     */
    protected mixed $value;

    /**
     * @param numeric $value
     */
    public function __construct(mixed $value) {
      parent::__construct();
      $this->value = $value;
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
      return $this->value < $other;
    }

    /**
     * Returns a string representation of the constraint.
     *
     * @return string
     */
    public function toString(): string {
      return 'is greater than ' . $this->exporter->export($this->value);
    }

}
