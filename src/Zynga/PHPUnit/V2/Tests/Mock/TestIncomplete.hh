<?hh

namespace Zynga\PHPUnit\V2\Tests\Mock;

use Zynga\PHPUnit\V2\TestCase;

class TestIncomplete extends TestCase {
  public function testMarkIncomplete() {
    $this->markTestIncomplete('Incomplete test');
  }
}
