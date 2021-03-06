<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;

class AssertAttributeEmpty {

  /**
   * Asserts that a static attribute of a class or an attribute of an object
   * is empty.
   *
   * @param string        $haystackAttributeName
   * @param string|object $haystackClassOrObject
   * @param string        $message
   *
   * @since Method available since Release 3.5.0
   */
  public static function evaluate(
    Assertions $assertions,
    string $haystackAttributeName,
    mixed $haystackClassOrObject,
    string $message = '',
  ): bool {

    return $assertions->assertEmpty(
      $assertions->readAttribute(
        $haystackClassOrObject,
        $haystackAttributeName,
      ),
      $message,
    );

  }

}
