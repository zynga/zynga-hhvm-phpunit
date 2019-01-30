<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;
use SebastianBergmann\PHPUnit\Constraints\IsInfiniteConstraint;

class AssertInfinite {

  /**
   * Asserts that a variable is infinite.
   *
   * @param mixed  $actual
   * @param string $message
   */
  public static function evaluate(
    Assertions $assertions,
    mixed $actual,
    string $message = '',
  ): bool {

    $constraint = new IsInfiniteConstraint();

    return $assertions->assertThat($actual, $constraint, $message);

  }

}
