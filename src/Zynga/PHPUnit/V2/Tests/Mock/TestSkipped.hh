<?hh

namespace Zynga\PHPUnit\V2\Tests\Mock;

use Zynga\PHPUnit\V2\TestCase;

class TestSkipped extends TestCase {
  public function testSkip() {
    $this->markTestSkipped('Skipped test');
  }
}
