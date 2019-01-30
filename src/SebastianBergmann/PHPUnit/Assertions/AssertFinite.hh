<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;
use SebastianBergmann\PHPUnit\Constraints\IsFiniteConstraint;

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
