<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;
use SebastianBergmann\PHPUnit\Constraints\IsEmptyConstraint;
use SebastianBergmann\PHPUnit\Constraints\NotConstraint;

class AssertNotEmpty {

  /**
   * Asserts that a variable is not empty.
   *
   * @param mixed  $actual
   * @param string $message
   *
   * @throws PHPUnit_Framework_AssertionFailedError
   */
  public static function evaluate(
    Assertions $assertions,
    mixed $actual,
    string $message = '',
  ): bool {

    $constraint = new IsEmptyConstraint();

    $notConstraint = new NotConstraint();
    $notConstraint->setExpected($constraint);

    return $assertions->assertThat($actual, $notConstraint, $message);

  }

}
