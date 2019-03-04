<?hh

namespace Zynga\PHPUnit\V2\Tests\Mock;

use Zynga\PHPUnit\V2\TestCase;

class Failure extends TestCase {
  public function testFailure() {
    $this->fail();
  }
}
