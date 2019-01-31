<?hh // strict

namespace SebastianBergmann\PHPUnit\Constraints;

use SebastianBergmann\PHPUnit\Constraints\CountConstraint;

class SameSizeConstraint extends CountConstraint {

  public function setExpected(mixed $value): bool {

    $countedValue = $this->getCountOf($value);

    return parent::setExpected($countedValue);

  }

}
