<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Constraints\NotConstraint;
use Zynga\PHPUnit\V2\Constraints\SameSizeConstraint;
use Zynga\PHPUnit\V2\Exceptions\InvalidArgumentExceptionFactory;

use \Countable;
use \Traversable;

class AssertNotSameSize {

  /**
   * Assert that the size of two arrays (or `Countable` or `Traversable` objects)
   * is not the same.
   *
   * @param array|Countable|Traversable $expected
   * @param array|Countable|Traversable $actual
   * @param string                      $message
   */
  public static function evaluate(
    Assertions $assertions,
    mixed $expected,
    mixed $actual,
    string $message = '',
  ): bool {

    if (!$expected instanceof Countable &&
        !$expected instanceof Traversable &&
        !is_array($expected) &&
        !$expected instanceof Map &&
        !$expected instanceof Vector) {
      throw InvalidArgumentExceptionFactory::factory(
        1,
        'countable, traversable, array, map or vector',
      );
    }

    if (!$actual instanceof Countable &&
        !$actual instanceof Traversable &&
        !is_array($actual) &&
        !$expected instanceof Map &&
        !$expected instanceof Vector) {
      throw InvalidArgumentExceptionFactory::factory(
        2,
        'countable, traversable, array, map or vector',
      );
    }

    $sameSize = new SameSizeConstraint();
    $sameSize->setExpected($expected);

    $constraint = new NotConstraint();
    $constraint->setExpected($sameSize);

    return $assertions->assertThat($actual, $constraint, $message);

  }

}
