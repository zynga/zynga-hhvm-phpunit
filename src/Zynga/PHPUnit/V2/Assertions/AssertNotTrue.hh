<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Constraints\IsFalseConstraint;

class AssertNotTrue {

  /**
   * Asserts that a condition is not true.
   *
   * @param bool   $condition
   * @param string $message
   *
   * @throws AssertionFailedException
   */
  public static function evaluate(
    Assertions $assertions,
    bool $condition,
    string $message = '',
  ): bool {

    $constraint = new IsFalseConstraint();

    return $assertions->assertThat($condition, $constraint, $message);

  }

}
