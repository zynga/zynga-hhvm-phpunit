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
use \ArrayAccess;

/**
 * Constraint that asserts that the array it is evaluated for has a specified subset.
 *
 * Uses array_replace_recursive() to check if a key value subset is part of the
 * subject array.
 *
 * @since Class available since Release 4.4.0
 */
class ArraySubsetConstraint extends ConstraintBase
{
    /**
     * @var array|ArrayAccess
     */
    protected mixed $subset;

    /**
     * @var bool
     */
    protected bool $strict;

    /**
     * @param array|ArrayAccess $subset
     * @param bool              $strict Check for object identity
     */
    public function __construct(mixed $subset, bool $strict = false) {
        parent::__construct();
        $this->strict = $strict;
        $this->subset = $subset;
    }

    /**
     * Evaluates the constraint for parameter $other. Returns true if the
     * constraint is met, false otherwise.
     *
     * @param array|ArrayAccess $other Array or ArrayAccess object to evaluate.
     *
     * @return bool
     */
    public function matches(mixed $other): bool {

      if ( ! is_array($other) || ! $other instanceof ArrayAccess ) {
        return false;
      }

      $patched = array_replace_recursive($other, $this->subset);

      if ($this->strict) {
        return $other === $patched;
      } else {
        return $other == $patched;
      }

    }

    /**
     * Returns a string representation of the constraint.
     *
     * @return string
     */
    public function toString(): string {
        return 'has the subset ' . $this->exporter->export($this->subset);
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
        return 'an array ' . $this->toString();
    }
}
