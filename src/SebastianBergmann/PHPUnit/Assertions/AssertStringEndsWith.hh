<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;
use SebastianBergmann\PHPUnit\Constraints\StringEndsWithConstraint;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentExceptionFactory;

class AssertStringEndsWith {

  /**
   * Asserts that a string ends with a given suffix.
   *
   * @param string $suffix
   * @param string $string
   * @param string $message
   *
   * @since Method available since Release 3.4.0
   */
  public static function evaluate(
    Assertions $assertions,
    string $suffix,
    string $string,
    string $message = '',
  ): bool {

    if (!is_string($suffix)) {
      throw InvalidArgumentExceptionFactory::factory(1, 'string');
    }

    if (!is_string($string)) {
      throw InvalidArgumentExceptionFactory::factory(2, 'string');
    }

    $constraint = new StringEndsWithConstraint();
    $constraint->setExpected($suffix);

    return $assertions->assertThat($string, $constraint, $message);

  }

}
