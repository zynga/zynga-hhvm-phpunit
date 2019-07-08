<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use SebastianBergmann\PHPUnit\Constraints\CountConstraint;
use SebastianBergmann\PHPUnit\Constraints\NotConstraint;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentExceptionFactory;
use \Countable;
use \Traversable;

class AssertNotCount {

  /**
   * Asserts the number of elements of an array, Countable or Traversable.
   *
   * @param int    $expectedCount
   * @param mixed  $haystack
   * @param string $message
   */
  public static function evaluate(
    Assertions $assertions,
    int $expectedCount,
    mixed $haystack,
    string $message = '',
  ): bool {

    if (!is_int($expectedCount)) {
      throw InvalidArgumentExceptionFactory::factory(1, 'integer');
    }

    if (!$haystack instanceof Countable &&
        !$haystack instanceof Traversable &&
        !is_array($haystack)) {
      throw InvalidArgumentExceptionFactory::factory(
        2,
        'countable or traversable',
      );
    }

    $countConstraint = new CountConstraint();
    $countConstraint->setExpected($expectedCount);

    $constraint = new NotConstraint();
    $constraint->setExpected($countConstraint);

    return $assertions->assertThat($haystack, $constraint, $message);

  }

}
