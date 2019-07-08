<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use SebastianBergmann\PHPUnit\Constraints\TraversableContainsOnlyConstraint;
use SebastianBergmann\PHPUnit\Environment\LanguageTypes;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentExceptionFactory;

class AssertContainsOnly {

  final static public function evaluate(
    Assertions $assertions,
    string $type,
    mixed $haystack,
    bool $isNativeType = false,
    string $message = '',
  ): bool {

    if (!(is_array($haystack) ||
          is_object($haystack) && $haystack instanceof Traversable)) {
      throw InvalidArgumentExceptionFactory::factory(
        2,
        'array or traversable',
      );
    }

    $isNativeType = LanguageTypes::isNativeType($type);

    $constraint = new TraversableContainsOnlyConstraint();
    $constraint->setExpected($type);
    $constraint->setIsNativeType($isNativeType);

    return $assertions->assertThat($haystack, $constraint, $message);

  }

}
