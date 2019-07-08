<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;

class AssertAttributeEquals {

  /**
   * Asserts that a variable is equal to an attribute of an object.
   *
   * @param mixed         $expected
   * @param string        $actualAttributeName
   * @param string|object $actualClassOrObject
   * @param string        $message
   * @param float         $delta
   * @param int           $maxDepth
   * @param bool          $canonicalize
   * @param bool          $ignoreCase
   */
  public static function evaluate(
    Assertions $assertions,
    mixed $expected,
    string $actualAttributeName,
    mixed $actualClassOrObject,
    string $message = '',
    float $delta = 0.0,
    int $maxDepth = 10,
    bool $canonicalize = false,
    bool $ignoreCase = false,
  ): bool {

    return $assertions->assertEquals(
      $expected,
      $assertions->readAttribute($actualClassOrObject, $actualAttributeName),
      $message,
      $delta,
      $maxDepth,
      $canonicalize,
      $ignoreCase,
    );
  }

}
