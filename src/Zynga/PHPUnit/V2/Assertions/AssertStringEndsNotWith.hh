<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Constraints\NotConstraint;
use Zynga\PHPUnit\V2\Constraints\StringEndsWithConstraint;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentExceptionFactory;

class AssertStringEndsNotWith {

  /**
   * Asserts that a string ends not with a given suffix.
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

    $stringEndsWithConstraint = new StringEndsWithConstraint();
    $stringEndsWithConstraint->setExpected($suffix);

    $constraint = new NotConstraint();
    $constraint->setExpected($stringEndsWithConstraint);

    return $assertions->assertThat($string, $constraint, $message);

  }

}
