<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Constraints\IsIdenticalConstraint;
class AssertSame {

  /**
   * Asserts that two variables have the same type and value.
   * Used on objects, it asserts that two variables reference
   * the same object.
   *
   * @param mixed  $expected
   * @param mixed  $actual
   * @param string $message
   */
  public static function evaluate(
    Assertions $assertions,
    mixed $expected,
    mixed $actual,
    string $message = '',
  ): bool {
    if (is_bool($expected) && is_bool($actual)) {
      return $assertions->assertEquals($expected, $actual, $message);
    }

    $constraint = new IsIdenticalConstraint();
    $constraint->setExpected($expected);

    return $assertions->assertThat($actual, $constraint, $message);

  }

}
