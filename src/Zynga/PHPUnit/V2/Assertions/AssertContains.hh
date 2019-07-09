<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Exceptions\InvalidArgumentExceptionFactory;
use Zynga\PHPUnit\V2\Constraints\StringContainsConstraint;
use Zynga\PHPUnit\V2\Constraints\TraversableContainsConstraint;

class AssertContains {

  /**
   * Asserts that a haystack contains a needle.
   *
   * @param mixed  $needle
   * @param mixed  $haystack
   * @param string $message
   * @param bool   $ignoreCase
   * @param bool   $checkForObjectIdentity
   * @param bool   $checkForNonObjectIdentity
   *
   * @since Method available since Release 2.1.0
   */
  final public static function evaluate(
    Assertions $assertions,
    mixed $needle,
    mixed $haystack,
    string $message = '',
    bool $ignoreCase = false,
    bool $checkForObjectIdentity = true,
    bool $checkForNonObjectIdentity = false,
  ): bool {

    if (is_array($haystack) ||
        $haystack instanceof Traversable ||
        $haystack instanceof Vector ||
        $haystack instanceof Map) {

      $constraint = new TraversableContainsConstraint();
      $constraint->setExpected($needle);
      $constraint->setCheckForObjectIdentity($checkForObjectIdentity);
      $constraint->setCheckForNonObjectIdentity($checkForNonObjectIdentity);

      return $assertions->assertThat($haystack, $constraint, $message);

    } else if (is_string($haystack)) {

      if (!is_string($needle)) {
        throw InvalidArgumentExceptionFactory::factory(1, 'string');
      }

      $constraint = new StringContainsConstraint();
      $constraint->setExpected($needle);
      $constraint->setIgnoreCase($ignoreCase);

      return $assertions->assertThat($haystack, $constraint, $message);

    }

    throw InvalidArgumentExceptionFactory::factory(
      2,
      'array, traversable or string',
    );

  }

}
