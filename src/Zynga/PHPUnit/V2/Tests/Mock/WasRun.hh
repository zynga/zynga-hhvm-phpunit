<?hh

namespace Zynga\PHPUnit\V2\Tests\Mock;

use Zynga\PHPUnit\V2\TestCase;

class WasRun extends TestCase {
  public $wasRun = false;

  public function testWasRun() {
    $this->wasRun = true;
  }

}
