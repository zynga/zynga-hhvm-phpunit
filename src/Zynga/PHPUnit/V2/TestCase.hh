<?hh // strict

namespace Zynga\PHPUnit\V2;

use Zynga\PHPUnit\V2\TestCase\Status;
use SebastianBergmann\PHPUnit\Assertions;

class TestCase extends Assertions {

  private Status $_status;

  public function __construct() {
    $this->_status = new Status();
  }

  public function status(): Status {
    return $this->_status;
  }

}
