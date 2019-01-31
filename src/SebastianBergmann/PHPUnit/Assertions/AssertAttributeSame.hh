<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;

class AssertAttributeSame {

  /**
   * Asserts that a variable and an attribute of an object have the same type
   * and value.
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

    return $assertions->assertSame(
      $expected,
      $assertions->readAttribute($actualClassOrObject, $actualAttributeName),
      $message,
    );

  }

}
