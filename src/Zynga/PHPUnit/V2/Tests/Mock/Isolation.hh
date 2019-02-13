<?hh

namespace Zynga\PHPUnit\V2\Tests\Mock;

use \PHPUnit_Framework_TestCase;

class Isolation extends PHPUnit_Framework_TestCase {
  public function testIsInIsolationReturnsFalse() {
    $this->assertFalse($this->isInIsolation());
  }

  public function testIsInIsolationReturnsTrue() {
    $this->assertTrue($this->isInIsolation());
  }
}
