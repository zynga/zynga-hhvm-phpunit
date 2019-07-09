<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Constraints\IsNullConstraint;
use Zynga\PHPUnit\V2\Constraints\NotConstraint;

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
