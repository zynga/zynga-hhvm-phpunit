<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use SebastianBergmann\PHPUnit\Constraints\IsNanConstraint;

class AssertNan {

  /**
   * Asserts that a variable is nan.
   *
   * @param mixed  $actual
   * @param string $message
   */
  public static function evaluate(
    Assertions $assertions,
    mixed $actual,
    string $message = '',
  ): bool {

    $constraint = new IsNanConstraint();

    return $assertions->assertThat($actual, $constraint, $message);

  }

}
