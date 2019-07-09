<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Constraints\IsTrueConstraint;

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
