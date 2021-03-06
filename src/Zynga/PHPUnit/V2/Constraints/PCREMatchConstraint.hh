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
 * Constraint that asserts that the string it is evaluated for matches
 * a regular expression.
 *
 * Checks a given value using the Perl Compatible Regular Expression extension
 * in PHP. The pattern is matched by executing preg_match().
 *
 * The pattern string passed in the constructor.
 *
 * @since Class available since Release 3.0.0
 */
class PCREMatchConstraint extends Base {

  protected string $pattern = '';

  public function setExpected(mixed $value): bool {
    if (is_string($value)) {
      $this->pattern = $value;
      return true;
    }
    return false;
  }

  public function resetExpected(): bool {
    $this->pattern = '';
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
    return preg_match($this->pattern, $other) > 0;
  }

  /**
   * Returns a string representation of the constraint.
   *
   * @return string
   */
  public function toString(): string {
    return sprintf('matches PCRE pattern "%s"', $this->pattern);
  }

}
