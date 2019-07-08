<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;

class AssertAttributeLessThanOrEqual {

  /**
   * Asserts that an attribute is smaller than or equal to another value.
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

    return $assertions->assertLessThanOrEqual(
      $expected,
      $assertions->readAttribute($actualClassOrObject, $actualAttributeName),
      $message,
    );

  }

}
