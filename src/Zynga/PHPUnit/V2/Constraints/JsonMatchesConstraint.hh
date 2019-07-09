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
 * Asserts whether or not two JSON objects are equal.
 *
 * @since Class available since Release 3.7.0
 */
class JsonMatchesConstraint extends Base {

  private string $value = '';

  public function setExpected(mixed $value): bool {
    $this->value = strval($value);
    return true;
  }

  public function resetExpected(): bool {
    $this->value = '';
    return true;
  }

  /**
   * Evaluates the constraint for parameter $other. Returns true if the
   * constraint is met, false otherwise.
   *
   * This method can be overridden to implement the evaluation algorithm.
   *
   * @param mixed $other Value or object to evaluate.
   *
   * @return bool
   */
  public function matches(mixed $other): bool {

    $decodedOther = json_decode($other);

    if (json_last_error()) {
      return false;
    }

    $decodedValue = json_decode($this->value);

    if (json_last_error()) {
      return false;
    }

    return $decodedOther == $decodedValue;

  }

  /**
   * Returns a string representation of the object.
   *
   * @return string
   */
  public function toString(): string {
    return sprintf('matches JSON string "%s"', $this->value);
  }
}
