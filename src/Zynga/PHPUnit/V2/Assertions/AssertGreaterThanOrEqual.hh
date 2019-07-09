<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Constraints\LogicalOrConstraint;
use Zynga\PHPUnit\V2\Constraints\GreaterThanConstraint;
use Zynga\PHPUnit\V2\Constraints\IsEqualConstraint;

class AssertGreaterThanOrEqual {

  /**
   * Asserts that a value is greater than or equal to another value.
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

    $isEqualConstraint = new IsEqualConstraint();
    $isEqualConstraint->setExpected($expected);

    $greaterThanConstraint = new GreaterThanConstraint();
    $greaterThanConstraint->setExpected($expected);

    $logicalOrConstraint = new LogicalOrConstraint();
    $logicalOrConstraint->addConstraint($isEqualConstraint);
    $logicalOrConstraint->addConstraint($greaterThanConstraint);

    return $assertions->assertThat($actual, $logicalOrConstraint, $message);

  }

}
