<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use SebastianBergmann\PHPUnit\Constraints\PCREMatchConstraint;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentExceptionFactory;

class AssertRegExp {

  /**
   * Asserts that a string matches a given regular expression.
   *
   * @param string $pattern
   * @param string $string
   * @param string $message
   */
  public static function evaluate(
    Assertions $assertions,
    string $pattern,
    string $string,
    string $message = '',
  ): bool {

    if (!is_string($pattern)) {
      throw InvalidArgumentExceptionFactory::factory(1, 'string');
    }

    if (!is_string($string)) {
      throw InvalidArgumentExceptionFactory::factory(2, 'string');
    }

    $constraint = new PCREMatchConstraint();
    $constraint->setExpected($pattern);

    return $assertions->assertThat($string, $constraint, $message);

  }

}
