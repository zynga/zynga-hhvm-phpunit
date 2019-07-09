<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Constraints\NotConstraint;
use Zynga\PHPUnit\V2\Constraints\StringStartsWithConstraint;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentExceptionFactory;

class AssertStringStartsNotWith {

  /**
   * Asserts that a string starts not with a given prefix.
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

    $startsWithConstraint = new StringStartsWithConstraint();
    $startsWithConstraint->setExpected($prefix);

    $constraint = new NotConstraint();
    $constraint->setExpected($startsWithConstraint);

    return $assertions->assertThat($string, $constraint, $message);

  }

}
