<?hh

namespace Zynga\PHPUnit\V2\Tests\Mock;

use \PHPUnit_Framework_TestCase;

class TestSkipped extends PHPUnit_Framework_TestCase {
  protected function runTest() {
    $this->markTestSkipped('Skipped test');
  }
}
