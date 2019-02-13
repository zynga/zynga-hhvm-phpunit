<?hh

namespace Zynga\PHPUnit\V2\Tests\Mock;

use \PHPUnit_Framework_TestCase;
use \Exception;

class ExceptionIn extends PHPUnit_Framework_TestCase {
  public $setUp = false;
  public $assertPreConditions = false;
  public $assertPostConditions = false;
  public $tearDown = false;
  public $testSomething = false;

  protected function setUp() {
    $this->setUp = true;
  }

  protected function assertPreConditions() {
    $this->assertPreConditions = true;
  }

  public function testSomething() {
    $this->testSomething = true;
    throw new Exception();
  }

  protected function assertPostConditions() {
    $this->assertPostConditions = true;
  }

  protected function tearDown() {
    $this->tearDown = true;
  }
}
