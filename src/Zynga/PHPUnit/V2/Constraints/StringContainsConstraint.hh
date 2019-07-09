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
 * Constraint that asserts that the string it is evaluated for contains
 * a given string.
 *
 * Uses strpos() to find the position of the string in the input, if not found
 * the evaluation fails.
 *
 * The sub-string is passed in the constructor.
 *
 * @since Class available since Release 3.0.0
 */
class StringContainsConstraint extends Base {
  /**
   * @var string
   */
  private string $string = '';

  /**
   * @var bool
   */
  private bool $ignoreCase = false;

  public function setIgnoreCase(bool $value): bool {
    $this->ignoreCase = $value;
    return true;
  }

  public function setExpected(mixed $string): bool {
    $this->string = strval($string);
    return true;
  }

  public function resetExpected(): bool {
    $this->string = '';
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
    if ($this->ignoreCase) {
      return stripos($other, $this->string) !== false;
    } else {
      return strpos($other, $this->string) !== false;
    }
  }

  /**
   * Returns a string representation of the constraint.
   *
   * @return string
   */
  public function toString(): string {
    if ($this->ignoreCase) {
      $string = strtolower($this->string);
    } else {
      $string = $this->string;
    }

    return sprintf('contains "%s"', $string);
  }
}
