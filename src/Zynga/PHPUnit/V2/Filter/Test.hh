<?hh // strict

namespace Zynga\PHPUnit\V2\Filter;

use Zynga\PHPUnit\V2\Interfaces\FilterInterface;
use Zynga\PHPUnit\V2\Interfaces\TestInterface;

class Test implements FilterInterface {
  private string $_regex;

  public function __construct(string $regex) {
    $this->_regex = $regex;
  }

  public function apply(TestInterface $test): bool {
    if (preg_match($this->_regex, $test->getName())) {
      return true;
    }
    return false;
  }

}
