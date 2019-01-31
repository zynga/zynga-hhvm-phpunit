<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;

class AssertAttributeNotSame {

  /**
   * Asserts that a variable and an attribute of an object do not have the
   * same type and value.
   *
   * @param mixed         $expected
   * @param string        $actualAttributeName
   * @param string|object $actualClassOrObject
   * @param string        $message
   */
  public static function evaluate(
    Assertions $assertions,
    mixed $expected,
    string $actualAttributeName,
    mixed $actualClassOrObject,
    string $message = '',
  ): bool {

    return $assertions->assertNotSame(
      $expected,
      $assertions->readAttribute($actualClassOrObject, $actualAttributeName),
      $message,
    );

  }

}
