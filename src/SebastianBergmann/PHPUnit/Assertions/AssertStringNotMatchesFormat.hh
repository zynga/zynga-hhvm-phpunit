<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;
use SebastianBergmann\PHPUnit\Constraints\NotConstraint;
use SebastianBergmann\PHPUnit\Constraints\StringMatchesConstraint;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentExceptionFactory;

class AssertStringNotMatchesFormat {

  /**
   * Asserts that a string does not match a given format string.
   *
   * @param string $format
   * @param string $string
   * @param string $message
   *
   * @since Method available since Release 3.5.0
   */
  public static function evaluate(
    Assertions $assertions,
    string $format,
    string $string,
    string $message = '',
  ): bool {

    if (!is_string($format)) {
      throw InvalidArgumentExceptionFactory::factory(1, 'string');
    }

    if (!is_string($string)) {
      throw InvalidArgumentExceptionFactory::factory(2, 'string');
    }

    $matcher = new StringMatchesConstraint();
    $matcher->setExpected($format);

    $constraint = new NotConstraint();
    $constraint->setExpected($matcher);

    return $assertions->assertThat($string, $constraint, $message);

  }

}
