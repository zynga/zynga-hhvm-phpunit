<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;
use SebastianBergmann\PHPUnit\Constraints\SameSizeConstraint;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentExceptionFactory;

use \Countable;
use \Traversable;

class AssertSameSize {

  /**
   * Assert that the size of two arrays (or `Countable` or `Traversable` objects)
   * is the same.
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
        !$actual instanceof Map &&
        !$actual instanceof Vector) {
      throw InvalidArgumentExceptionFactory::factory(
        2,
        'countable, traversable, array, map or vector',
      );
    }

    $constraint = new SameSizeConstraint();
    $constraint->setExpected($expected);

    return $assertions->assertThat($actual, $constraint, $message);

  }

}
