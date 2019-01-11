<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;
use SebastianBergmann\PHPUnit\Constraints\NotConstraint;
use SebastianBergmann\PHPUnit\Constraints\StringContainsConstraint;
use SebastianBergmann\PHPUnit\Constraints\TraversableContainsConstraint;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentExceptionFactory;

class AssertNotContains {

  /**
   * Asserts that a haystack does not contain a needle.
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
  final static public function evaluate(
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

      $traversableConstraint = new TraversableContainsConstraint();

      $traversableConstraint->setExpected($needle);
      $traversableConstraint->setCheckForObjectIdentity(
        $checkForObjectIdentity,
      );
      $traversableConstraint->setCheckForNonObjectIdentity(
        $checkForNonObjectIdentity,
      );

      $notConstraint = new NotConstraint();
      $notConstraint->setExpected($traversableConstraint);

      return $assertions->assertThat($haystack, $notConstraint, $message);

    } else if (is_string($haystack)) {

      if (!is_string($needle)) {
        throw InvalidArgumentExceptionFactory::factory(1, 'string');
      }

      $stringContainsConstraint = new StringContainsConstraint();

      $stringContainsConstraint->setExpected($needle);
      $stringContainsConstraint->setIgnoreCase($ignoreCase);

      $notConstraint = new NotConstraint();
      $notConstraint->setExpected($stringContainsConstraint);

      return $assertions->assertThat($haystack, $notConstraint, $message);

    }

    throw InvalidArgumentExceptionFactory::factory(
      2,
      'array, traversable or string',
    );

  }

}
