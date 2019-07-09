<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Constraints\IsEmptyConstraint;

class AssertEmpty {

  /**
   * Asserts that a variable is empty.
   *
   * @param mixed  $actual
   * @param string $message
   *
   * @throws AssertionFailedException
   */
  public static function evaluate(
    Assertions $assertions,
    mixed $actual,
    string $message = '',
  ): bool {

    $constraint = new IsEmptyConstraint();

    return $assertions->assertThat($actual, $constraint, $message);

  }

}
