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
 * Constraint that asserts that the string it is evaluated for ends with a given
 * suffix.
 *
 * @since Class available since Release 3.4.0
 */
class StringEndsWithConstraint extends ConstraintBase {

    /**
     * @var string
     */
    protected string $suffix;

    /**
     * @param string $suffix
     */
    public function __construct(mixed $suffix) {
      parent::__construct();
      $this->suffix = strval($suffix);
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
      $other = strval($other);
      return substr($other, 0 - strlen($this->suffix)) == $this->suffix;
    }

    /**
     * Returns a string representation of the constraint.
     *
     * @return string
     */
    public function toString(): string {
      return 'ends with "' . $this->suffix . '"';
    }
}
