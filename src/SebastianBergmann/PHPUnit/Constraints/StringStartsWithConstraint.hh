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
 * Constraint that asserts that the string it is evaluated for begins with a
 * given prefix.
 *
 * @since Class available since Release 3.4.0
 */
class StringStartsWithConstraint extends Base {

  protected string $prefix = '';

  public function setExpected(mixed $value): bool {
    $this->prefix = strval($value);
    return true;
  }

  public function resetExpected(): bool {
    $this->prefix = '';
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
    return strpos($other, $this->prefix) === 0;
  }

  /**
   * Returns a string representation of the constraint.
   *
   * @return string
   */
  public function toString(): string {
    return 'starts with "'.$this->prefix.'"';
  }

}
