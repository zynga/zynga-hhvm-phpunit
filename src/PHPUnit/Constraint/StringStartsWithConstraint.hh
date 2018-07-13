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
 * Constraint that asserts that the string it is evaluated for begins with a
 * given prefix.
 *
 * @since Class available since Release 3.4.0
 */
class StringStartsWithConstraint extends ConstraintBase {

    /**
     * @var string
     */
    protected string $prefix;

    /**
     * @param string $prefix
     */
    public function __construct(mixed $prefix) {
      parent::__construct();
      $this->prefix = strval($prefix);
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
      return strpos($other, $this->prefix) === 0;
    }

    /**
     * Returns a string representation of the constraint.
     *
     * @return string
     */
    public function toString(): string {
      return 'starts with "' . $this->prefix . '"';
    }
}
