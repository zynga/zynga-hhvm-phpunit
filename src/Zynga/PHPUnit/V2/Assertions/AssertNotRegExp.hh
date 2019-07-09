<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Constraints\NotConstraint;
use Zynga\PHPUnit\V2\Constraints\PCREMatchConstraint;
use Zynga\PHPUnit\V2\Exceptions\InvalidArgumentExceptionFactory;

class AssertNotRegExp {

  /**
   * Asserts that a string does not match a given regular expression.
   *
   * @param string $pattern
   * @param string $string
   * @param string $message
   *
   * @since Method available since Release 2.1.0
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

    $pcreMatch = new PCREMatchConstraint();
    $pcreMatch->setExpected($pattern);

    $constraint = new NotConstraint();
    $constraint->setExpected($pcreMatch);

    return $assertions->assertThat($string, $constraint, $message);

  }

}
