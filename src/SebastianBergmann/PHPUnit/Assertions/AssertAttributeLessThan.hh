<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;

class AssertAttributeLessThan {

  /**
   * Asserts that an attribute is smaller than another value.
   *
   * @param mixed         $expected
   * @param string        $actualAttributeName
   * @param string|object $actualClassOrObject
   * @param string        $message
   *
   * @since Method available since Release 3.1.0
   */
  public static function evaluate(
    Assertions $assertions,
    mixed $expected,
    string $actualAttributeName,
    mixed $actualClassOrObject,
    string $message = '',
  ): bool {

    return $assertions->assertLessThan(
      $expected,
      $assertions->readAttribute($actualClassOrObject, $actualAttributeName),
      $message,
    );

  }

}
