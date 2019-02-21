<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;
use SebastianBergmann\PHPUnit\Constraints\IsTrueConstraint;

class AssertClearStatCache {

  public static function evaluate(
    Assertions $assertions,
    string $message = '',
  ): bool {

    $constraint = new IsTrueConstraint();

    return $assertions->assertThat(clearstatcache(), $constraint, $message);

  }

}
