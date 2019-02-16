<?hh

namespace Zynga\PHPUnit\V2\Tests\Mock;

use \PHPUnit_Framework_TestCase;
use \Exception;

class ExceptionInButExpected extends PHPUnit_Framework_TestCase {
  public $setUp = false;
  public $assertPreConditions = false;
  public $assertPostConditions = false;
  public $tearDown = false;
  public $testSomething = false;

  public function setUp() {
    $this->setUp = true;
  }

  public function assertPreConditions() {
    $this->assertPreConditions = true;
  }

  /**
   * @expectedException Exception
   * @expectedExceptionMessage expected message
   * @expectedExceptionMessageRegExp /mess/
   * @expectedExceptionCode 42
   */
  public function testSomething() {
    $this->testSomething = true;
    throw new Exception('expected message', 42);
  }

  public function assertPostConditions() {
    $this->assertPostConditions = true;
  }

  public function tearDown() {
    $this->tearDown = true;
  }

}
