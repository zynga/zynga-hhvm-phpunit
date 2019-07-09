<?hh // strict

namespace Zynga\PHPUnit\V2\Constraints;

use Zynga\PHPUnit\V2\Constraints\CountConstraint;

class SameSizeConstraint extends CountConstraint {

  public function setExpected(mixed $value): bool {

    $countedValue = $this->getCountOf($value);

    return parent::setExpected($countedValue);

  }

}
