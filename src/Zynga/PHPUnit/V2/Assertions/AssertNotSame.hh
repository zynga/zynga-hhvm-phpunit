<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use SebastianBergmann\PHPUnit\Constraints\IsIdenticalConstraint;
use SebastianBergmann\PHPUnit\Constraints\NotConstraint;

class AssertNotSame {

  /**
   * Asserts that two variables do not have the same type and value.
   * Used on objects, it asserts that two variables do not reference
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
      return $assertions->assertNotEquals($expected, $actual, $message);
    }

    $constraint = new IsIdenticalConstraint();
    $constraint->setExpected($expected);

    $notConstraint = new NotConstraint();
    $notConstraint->setExpected($constraint);

    return $assertions->assertThat($actual, $notConstraint, $message);

  }

}
