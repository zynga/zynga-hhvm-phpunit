<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;
use SebastianBergmann\PHPUnit\Constraints\IsNullConstraint;
use SebastianBergmann\PHPUnit\Constraints\NotConstraint;

class AssertNotNull {

  /**
   * Asserts that a variable is not null.
   *
   * @param mixed  $actual
   * @param string $message
   */
  public static function evaluate(
    Assertions $assertions,
    mixed $actual,
    string $message = '',
  ): bool {

    $isNullConstraint = new IsNullConstraint();

    $constraint = new NotConstraint();
    $constraint->setExpected($isNullConstraint);

    return $assertions->assertThat($actual, $constraint, $message);

  }

}
