<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use SebastianBergmann\PHPUnit\Constraints\IsNullConstraint;

class AssertNull {

  /**
   * Asserts that a variable is null.
   *
   * @param mixed  $actual
   * @param string $message
   */
  public static function evaluate(
    Assertions $assertions,
    mixed $actual,
    string $message = '',
  ): bool {

    $constraint = new IsNullConstraint();

    return $assertions->assertThat($actual, $constraint, $message);

  }

}
