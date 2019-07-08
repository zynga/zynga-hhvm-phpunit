<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;

class AssertAttributeContains {

  /**
   * Asserts that a haystack that is stored in a static attribute of a class
   * or an attribute of an object contains a needle.
   *
   * @param mixed         $needle
   * @param string        $haystackAttributeName
   * @param string|object $haystackClassOrObject
   * @param string        $message
   * @param bool          $ignoreCase
   * @param bool          $checkForObjectIdentity
   * @param bool          $checkForNonObjectIdentity
   *
   * @since Method available since Release 3.0.0
   */
  final public static function evaluate(
    Assertions $assertions,
    mixed $needle,
    string $haystackAttributeName,
    mixed $haystackClassOrObject,
    string $message = '',
    bool $ignoreCase = false,
    bool $checkForObjectIdentity = true,
    bool $checkForNonObjectIdentity = false,
  ): bool {

    $haystack = $assertions->readAttribute(
      $haystackClassOrObject,
      $haystackAttributeName,
    );

    return $assertions->assertContains(
      $needle,
      $haystack,
      $message,
      $ignoreCase,
      $checkForObjectIdentity,
      $checkForNonObjectIdentity,
    );

  }

}
