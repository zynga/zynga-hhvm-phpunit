<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;
use SebastianBergmann\PHPUnit\Exceptions\AssertionFailedException;
use SebastianBergmann\PHPUnit\Exceptions\ExpectationFailedException;
use SebastianBergmann\PHPUnit\Interfaces\ConstraintInterface;

class AssertThat {

  /**
   * Evaluates a PHPUnit_Framework_Constraint matcher object.
   *
   * @param mixed                        $value
   * @param PHPUnit_Framework_Constraint $constraint
   * @param string                       $message
   *
   * @since Method available since Release 3.0.0
   */
  final static public function evaluate(
    Assertions $assertions,
    mixed $value,
    ConstraintInterface $constraint,
    string $message = '',
  ): bool {

    $assertions->count()->increment($constraint->count());

    try {
      return $constraint->evaluate($value, $message, true);
    } catch (ExpectationFailedException $e) {
      throw new AssertionFailedException($e->getMessage(), $e->getCode(), $e);
    }

  }

}
