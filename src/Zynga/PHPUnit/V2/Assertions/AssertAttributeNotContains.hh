<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;

class AssertAttributeNotContains {

  final public static function evaluate(
    Assertions $assertions,
    mixed $needle,
    string $haystackAttributeName,
    mixed $haystackClassOrObject,
    string $message = '',
    bool $ignoreCase = false,
    bool $checkForObjectIdentity = true,
    bool $checkForNonObjectIdentity = false,
  ): bool {
    return $assertions->assertNotContains(
      $needle,
      $assertions->readAttribute(
        $haystackClassOrObject,
        $haystackAttributeName,
      ),
      $message,
      $ignoreCase,
      $checkForObjectIdentity,
      $checkForNonObjectIdentity,
    );
  }

}
