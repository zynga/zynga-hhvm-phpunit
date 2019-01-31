<?hh // strict

namespace SebastianBergmann\PHPUnit\Constraints;

use SebastianBergmann\PHPUnit\Constraints\Base;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use \Countable;
use \IteratorAggregate;

/**
 * @since Class available since Release 3.6.0
 */
class CountConstraint extends Base {

  private int $expectedCount = 0;

  public function setExpected(mixed $count): bool {
    $this->expectedCount = intval($count);
    return true;
  }

  public function resetExpected(): bool {
    $this->expectedCount = 0;
    return true;
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

    if ($other instanceof Countable) {
      return count($other);
    }

    if (is_array($other)) {
      return count($other);
    }

    if (!$other instanceof Traversable) {
      return -1;
    }

    // if ($other instanceof IteratorAggregate) {
    //   $iterator = $other->getIterator();
    // } else {
    //   $iterator = $other;
    // }
    //
    // $key = $iterator->key();
    $count = iterator_count($other);

    // manually rewind $iterator to previous key, since iterator_count
    // moves pointer
    // if ($key !== null) {
    //   $iterator->rewind();
    //   while ($iterator->valid() && $key !== $iterator->key()) {
    //     $iterator->next();
    //   }
    // }

    return $count;

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
      $this->expectedCount,
    );
  }

  public function toString(): string {
    return sprintf('count matches %d', $this->expectedCount);
  }

}
