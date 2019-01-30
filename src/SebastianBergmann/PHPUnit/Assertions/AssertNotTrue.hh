<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;
use SebastianBergmann\PHPUnit\Constraints\IsFalseConstraint;

class AssertNotTrue {

  /**
   * Asserts that a condition is not true.
   *
   * @param bool   $condition
   * @param string $message
   *
   * @throws PHPUnit_Framework_AssertionFailedError
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
