<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;
use SebastianBergmann\PHPUnit\Constraints\IsEmptyConstraint;

class AssertEmpty {

  /**
   * Asserts that a variable is empty.
   *
   * @param mixed  $actual
   * @param string $message
   *
   * @throws AssertionFailedException
   */
  public static function evaluate(
    Assertions $assertions,
    mixed $actual,
    string $message = '',
  ): bool {

    $constraint = new IsEmptyConstraint();

    return $assertions->assertThat($actual, $constraint, $message);

  }

}
