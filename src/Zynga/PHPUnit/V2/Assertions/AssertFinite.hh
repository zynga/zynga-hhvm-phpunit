<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Constraints\IsFiniteConstraint;

class AssertFinite {

  /**
   * Asserts that a variable is finite.
   *
   * @param mixed  $actual
   * @param string $message
   */
  public static function evaluate(
    Assertions $assertions,
    mixed $actual,
    string $message = '',
  ): bool {

    $constraint = new IsFiniteConstraint();

    return $assertions->assertThat($actual, $constraint, $message);

  }

}
