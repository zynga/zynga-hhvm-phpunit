<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Constraints\IsEqualConstraint;
use Zynga\PHPUnit\V2\Constraints\LessThanConstraint;
use Zynga\PHPUnit\V2\Constraints\OrConstraint;

class AssertLessThanOrEqual {

  /**
   * Asserts that a value is smaller than or equal to another value.
   *
   * @param mixed  $expected
   * @param mixed  $actual
   * @param string $message
   *
   * @since Method available since Release 3.1.0
   */
  public static function evaluate(
    Assertions $assertions,
    mixed $expected,
    mixed $actual,
    string $message = '',
  ): bool {

    $lessThanConstraint = new LessThanConstraint();
    $lessThanConstraint->setExpected($expected);

    $isEqualConstraint = new IsEqualConstraint();
    $isEqualConstraint->setExpected($expected);

    $orConstraint = new OrConstraint();
    $orConstraint->addConstraint($lessThanConstraint);
    $orConstraint->addConstraint($isEqualConstraint);

    return $assertions->assertThat($actual, $orConstraint, $message);

  }

}
