<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Constraints\StringStartsWithConstraint;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentExceptionFactory;

class AssertStringStartsWith {

  /**
   * Asserts that a string starts with a given prefix.
   *
   * @param string $prefix
   * @param string $string
   * @param string $message
   *
   * @since Method available since Release 3.4.0
   */
  public static function evaluate(
    Assertions $assertions,
    string $prefix,
    string $string,
    string $message = '',
  ): bool {

    if (!is_string($prefix)) {
      throw InvalidArgumentExceptionFactory::factory(1, 'string');
    }

    if (!is_string($string)) {
      throw InvalidArgumentExceptionFactory::factory(2, 'string');
    }

    $constraint = new StringStartsWithConstraint();
    $constraint->setExpected($prefix);

    return $assertions->assertThat($string, $constraint, $message);

  }

}
