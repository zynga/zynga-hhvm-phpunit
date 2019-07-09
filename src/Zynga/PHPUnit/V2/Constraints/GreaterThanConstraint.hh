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

/**
 * Constraint that asserts that the value it is evaluated for is greater
 * than a given value.
 *
 * @since Class available since Release 3.0.0
 */
class GreaterThanConstraint extends Base {

  protected mixed $value = null;

  public function setExpected(mixed $value): bool {
    $this->value = $value;
    return true;
  }

  public function resetExpected(): bool {
    $this->value = null;
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
    return $this->value < $other;
  }

  /**
   * Returns a string representation of the constraint.
   *
   * @return string
   */
  public function toString(): string {
    return 'is greater than '.$this->getExporter()->export($this->value);
  }

}
