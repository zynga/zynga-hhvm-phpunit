<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Constraints\IsTrueConstraint;

class AssertClearStatCache {

  public static function evaluate(
    Assertions $assertions,
    string $message = '',
  ): bool {

    $constraint = new IsTrueConstraint();

    return $assertions->assertThat(clearstatcache(), $constraint, $message);

  }

}
