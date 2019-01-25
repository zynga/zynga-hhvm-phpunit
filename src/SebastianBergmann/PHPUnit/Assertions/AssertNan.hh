<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;
use SebastianBergmann\PHPUnit\Constraints\IsNanConstraint;

class AssertNan {

  /**
   * Asserts that a variable is nan.
   *
   * @param mixed  $actual
   * @param string $message
   */
  public static function evaluate(
    Assertions $assertions,
    mixed $actual,
    string $message = '',
  ): bool {

    $constraint = new IsNanConstraint();

    return $assertions->assertThat($actual, $constraint, $message);

  }

}
