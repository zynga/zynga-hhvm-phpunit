<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;
use SebastianBergmann\PHPUnit\Constraints\IsEqualConstraint;
use SebastianBergmann\PHPUnit\Constraints\NotConstraint;

class AssertNotEquals {

  /**
   * Asserts that two variables are not equal.
   *
   * @param mixed  $expected
   * @param mixed  $actual
   * @param string $message
   * @param float  $delta
   * @param int    $maxDepth
   * @param bool   $canonicalize
   * @param bool   $ignoreCase
   *
   * @since Method available since Release 2.3.0
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

    $isEqualConstraint = new IsEqualConstraint();
    $isEqualConstraint->setExpected($expected);
    $isEqualConstraint->setDelta($delta);
    $isEqualConstraint->setMaxDepth($maxDepth);
    $isEqualConstraint->setCanonicalize($canonicalize);
    $isEqualConstraint->setIgnoreCase($ignoreCase);

    $notConstraint = new NotConstraint();
    $notConstraint->setExpected($isEqualConstraint);

    return $assertions->assertThat($actual, $notConstraint, $message);

  }

}
