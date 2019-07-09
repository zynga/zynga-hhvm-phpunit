<?hh // strict

namespace Zynga\PHPUnit\V2\Constraints;

use Zynga\PHPUnit\V2\Constraints\Base;

class FileExistsConstraint extends Base {

  public function setExpected(mixed $value): bool {
    return true;
  }

  public function resetExpected(): bool {
    return true;
  }

  public function matches(mixed $value): bool {

    if (is_file($value) && is_readable($value)) {
      return true;
    }

    return false;

  }

  public function toString(): string {
    return 'file exists';
  }

}
