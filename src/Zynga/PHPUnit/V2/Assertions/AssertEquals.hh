<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use SebastianBergmann\PHPUnit\Constraints\IsEqualConstraint;

class AssertEquals {

  /**
   * Asserts that two variables are equal.
   *
   * @param mixed  $expected
   * @param mixed  $actual
   * @param string $message
   * @param float  $delta
   * @param int    $maxDepth
   * @param bool   $canonicalize
   * @param bool   $ignoreCase
   */
  public static function evaluate(
    Assertions $assertions,
    mixed $expected,
    mixed $actual,
    string $message = '',
    float $delta = 0.0,
    int $maxDepth = 10,
    bool $canonicalize = false,
    bool $ignoreCase = false,
  ): bool {

    $constraint = new IsEqualConstraint();
    $constraint->setExpected($expected);
    $constraint->setDelta($delta);
    $constraint->setMaxDepth($maxDepth);
    $constraint->setCanonicalize($canonicalize);
    $constraint->setIgnoreCase($ignoreCase);

    return $assertions->assertThat($actual, $constraint, $message);

  }

}
