<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;
use SebastianBergmann\PHPUnit\Constraints\IsNativeTypeConstraint;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentExceptionFactory;

class AssertInternalType {

  /**
   * Asserts that a variable is of a given type.
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

    $constraint = new IsNativeTypeConstraint();
    $constraint->setExpected($expected);

    return $assertions->assertThat($actual, $constraint, $message);

  }

}
