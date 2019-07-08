<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;

class AssertAttributeInstanceOf {

  /**
   * Asserts that an attribute is of a given type.
   *
   * @param string        $expected
   * @param string        $attributeName
   * @param string|object $classOrObject
   * @param string        $message
   *
   * @since Method available since Release 3.5.0
   */
  public static function evaluate(
    Assertions $assertions,
    string $expected,
    string $attributeName,
    mixed $classOrObject,
    string $message = '',
  ): bool {

    return $assertions->assertInstanceOf(
      $expected,
      $assertions->readAttribute($classOrObject, $attributeName),
      $message,
    );

  }

}
