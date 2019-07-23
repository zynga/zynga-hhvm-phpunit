<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Constraints\IsInstanceOfConstraint;
use Zynga\PHPUnit\V2\Exceptions\InvalidArgumentExceptionFactory;

class AssertInstanceOf {

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

    if (!(is_string($expected) &&
          (class_exists($expected) || interface_exists($expected)))) {
      throw InvalidArgumentExceptionFactory::factory(
        1,
        'class or interface name',
      );
    }

    $constraint = new IsInstanceOfConstraint();
    // error_log('JEO expected=' . $expected);
    $constraint->setExpected($expected);

    return $assertions->assertThat($actual, $constraint, $message);

  }

}
