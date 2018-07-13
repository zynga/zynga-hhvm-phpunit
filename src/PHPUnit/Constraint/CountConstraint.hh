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
use \Countable;
use \IteratorAggregate;

/**
 * @since Class available since Release 3.6.0
 */
class CountConstraint extends ConstraintBase {

    /**
     * @var int
     */
    protected int $expectedCount = 0;

    /**
     * @param int $expected
     */
    public function __construct(mixed $expected) {
      parent::__construct();
      $this->expectedCount = intval($expected);
    }

    /**
     * Evaluates the constraint for parameter $other. Returns true if the
     * constraint is met, false otherwise.
     *
     * @param mixed $other
     *
     * @return bool
     */
    public function matches(mixed $other): bool {
      return $this->expectedCount === $this->getCountOf($other);
    }

    /**
     * @param mixed $other
     *
     * @return bool
     */
    public function getCountOf(mixed $other): int {

      if ($other instanceof Countable || is_array($other)) {
        return count($other);
      } elseif ($other instanceof IteratorAggregate) {
        $iterator = $other->getIterator();

        $count = 0;
        while($iterator->valid()) {
          $count++;
          $iterator->next();
        }

        return $count;
      }

      // Not an array, and not a iterator compatible item?
      return -1;

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
      return sprintf(
        'actual size %d matches expected size %d',
        $this->getCountOf($other),
        $this->expectedCount
      );
    }

    /**
     * @return string
     */
    public function toString(): string {
      return sprintf(
        'count matches %d',
        $this->expectedCount
      );
    }

}
