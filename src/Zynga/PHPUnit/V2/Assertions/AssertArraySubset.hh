<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Constraints\ArraySubsetConstraint;
use Zynga\PHPUnit\V2\Exceptions\InvalidArgumentExceptionFactory;

class AssertArraySubset {

  final static public function evaluate(
    Assertions $assertions,
    mixed $subset,
    mixed $array,
    bool $strict = false,
    string $message = '',
  ): bool {

    if (!is_array($subset)) {
      throw InvalidArgumentExceptionFactory::factory(
        1,
        'array or ArrayAccess',
      );
    }

    if (!is_array($array)) {
      throw InvalidArgumentExceptionFactory::factory(
        2,
        'array or ArrayAccess',
      );
    }

    $constraint = new ArraySubsetConstraint();

    $constraint->setExpected($subset);

    $constraint->setStrict($strict);

    return $assertions->assertThat($array, $constraint, $message);

  }

}
