<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;
use SebastianBergmann\PHPUnit\Constraints\IsTrueConstraint;

class AssertTrue {

  /**
   * Asserts that a condition is true.
   *
   * @param bool   $condition
   * @param string $message
   *
   * @throws AssertionFailedException
   */
  final public static function evaluate(
    Assertions $assertions,
    bool $condition,
    string $message = '',
  ): bool {
    $constraint = new IsTrueConstraint();
    return $assertions->assertThat($condition, $constraint, $message);
  }

}
