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

  public function setUp() {
    $this->setUp = true;
  }

  public function assertPreConditions() {
    $this->assertPreConditions = true;
  }

  public function testSomething() {
    $this->testSomething = true;
    throw new Exception();
  }

  public function assertPostConditions() {
    $this->assertPostConditions = true;
  }

  public function tearDown() {
    $this->tearDown = true;
  }

}
