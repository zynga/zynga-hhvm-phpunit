<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Constraints\NotConstraint;
use Zynga\PHPUnit\V2\Constraints\TraversableContainsOnlyConstraint;
use Zynga\PHPUnit\V2\Exceptions\InvalidArgumentExceptionFactory;
use Zynga\PHPUnit\V2\Environment\LanguageTypes;

class AssertNotContainsOnly {
  /**
   * Asserts that a haystack does not contain only values of a given type.
   *
   * @param string $type
   * @param mixed  $haystack
   * @param bool   $isNativeType
   * @param string $message
   *
   * @since Method available since Release 3.1.4
   */
  final public static function evaluate(
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

    if ($isNativeType === false) {
      $isNativeType = LanguageTypes::isNativeType($type);
    }

    $constraint = new TraversableContainsOnlyConstraint();
    $constraint->setExpected($type);
    $constraint->setIsNativeType($isNativeType);

    $notConstraint = new NotConstraint();
    $notConstraint->setExpected($constraint);

    return $assertions->assertThat($haystack, $notConstraint, $message);

  }

}
