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
use \Countable;

/**
 * Constraint that checks whether a variable is empty().
 *
 * @since Class available since Release 3.5.0
 */
class IsEmptyConstraint extends Base {

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

    if ($other instanceof Countable) {
      return count($other) === 0;
    }

    if (is_string($other) && $other == '') {
      return true;
    }

    if (is_array($other) && count($other) == 0) {
      return true;
    }

    if ($other instanceof Map && $other->count() == 0) {
      return true;
    }

    if ($other instanceof Vector && $other->count() == 0) {
      return true;
    }

    return false;

    // return empty($other);
  }

  /**
   * Returns a string representation of the constraint.
   *
   * @return string
   */
  public function toString(): string {
    return 'is empty';
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
    $type = gettype($other);

    return sprintf(
      '%s %s %s',
      $type[0] == 'a' || $type[0] == 'o' ? 'an' : 'a',
      $type,
      $this->toString(),
    );
  }

}
