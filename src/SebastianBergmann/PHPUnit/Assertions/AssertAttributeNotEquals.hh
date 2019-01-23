<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;

class AssertAttributeNotEquals {

  /**
   * Asserts that a variable is not equal to an attribute of an object.
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

    return $assertions->assertNotEquals(
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
