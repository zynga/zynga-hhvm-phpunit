<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;

class AssertAttributeNotCount {

  /**
   * Asserts the number of elements of an array, Countable or Traversable
   * that is stored in an attribute.
   *
   * @param int           $expectedCount
   * @param string        $haystackAttributeName
   * @param string|object $haystackClassOrObject
   * @param string        $message
   *
   * @since Method available since Release 3.6.0
   */
  public static function evaluate(
    Assertions $assertions,
    int $expectedCount,
    string $haystackAttributeName,
    mixed $haystackClassOrObject,
    string $message = '',
  ): bool {

    return $assertions->assertNotCount(
      $expectedCount,
      $assertions->readAttribute(
        $haystackClassOrObject,
        $haystackAttributeName,
      ),
      $message,
    );

  }

}
