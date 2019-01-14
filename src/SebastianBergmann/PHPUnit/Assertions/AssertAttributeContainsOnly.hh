<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;

class AssertAttributeContainsOnly {
  /**
   * Asserts that a haystack that is stored in a static attribute of a class
   * or an attribute of an object contains only values of a given type.
   *
   * @param string        $type
   * @param string        $haystackAttributeName
   * @param string|object $haystackClassOrObject
   * @param bool          $isNativeType
   * @param string        $message
   *
   * @since Method available since Release 3.1.4
   */
  final static public function evaluate(
    Assertions $assertions,
    string $type,
    string $haystackAttributeName,
    mixed $haystackClassOrObject,
    bool $isNativeType = false,
    string $message = '',
  ): bool {

    return $assertions->assertContainsOnly(
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
