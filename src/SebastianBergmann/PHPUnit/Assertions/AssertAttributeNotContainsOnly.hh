<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;

class AssertAttributeNotContainsOnly {

  /**
   * Asserts that a haystack that is stored in a static attribute of a class
   * or an attribute of an object does not contain only values of a given
   * type.
   *
   * @param string        $type
   * @param string        $haystackAttributeName
   * @param string|object $haystackClassOrObject
   * @param bool          $isNativeType
   * @param string        $message
   *
   * @since Method available since Release 3.1.4
   */
  final public static function evaluate(
    Assertions $assertions,
    string $type,
    string $haystackAttributeName,
    mixed $haystackClassOrObject,
    bool $isNativeType = false,
    string $message = '',
  ): bool {

    return $assertions->assertNotContainsOnly(
      $type,
      $assertions->readAttribute(
        $haystackClassOrObject,
        $haystackAttributeName,
      ),
      $isNativeType,
      $message,
    );

  }

}
