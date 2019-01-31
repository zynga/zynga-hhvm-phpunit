<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;
use SebastianBergmann\PHPUnit\Constraints\IsNativeTypeConstraint;
use SebastianBergmann\PHPUnit\Constraints\NotConstraint;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentExceptionFactory;

class AssertNotInternalType {

  /**
   * Asserts that a variable is not of a given type.
   *
   * @param string $expected
   * @param mixed  $actual
   * @param string $message
   *
   * @since Method available since Release 3.5.0
   */
  public static function evaluate(
    Assertions $assertions,
    string $expected,
    mixed $actual,
    string $message = '',
  ): bool {

    if (!is_string($expected)) {
      throw InvalidArgumentExceptionFactory::factory(1, 'string');
    }

    $isNativeTypeConstraint = new IsNativeTypeConstraint();
    $isNativeTypeConstraint->setExpected($expected);

    $constraint = new NotConstraint();
    $constraint->setExpected($isNativeTypeConstraint);

    return $assertions->assertThat($actual, $constraint, $message);

  }

}
