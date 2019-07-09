<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Constraints\TraversableContainsOnlyConstraint;
//use SebastianBergmann\PHPUnit\Environment\LanguageTypes;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentExceptionFactory;

class AssertContainsOnlyInstancesOf {

  /**
   * Asserts that a haystack contains only instances of a given classname
   *
   * @param string            $classname
   * @param array|Traversable $haystack
   * @param string            $message
   */
  final public static function evaluate(
    Assertions $assertions,
    string $classname,
    mixed $haystack,
    string $message = '',
  ): bool {

    if (!(is_array($haystack) ||
          is_object($haystack) && $haystack instanceof Traversable)) {
      throw InvalidArgumentExceptionFactory::factory(
        2,
        'array or traversable',
      );
    }

    $constraint = new TraversableContainsOnlyConstraint();
    $constraint->setExpected($classname);
    $constraint->setIsNativeType(false);

    return $assertions->assertThat($haystack, $constraint, $message);

  }

}
