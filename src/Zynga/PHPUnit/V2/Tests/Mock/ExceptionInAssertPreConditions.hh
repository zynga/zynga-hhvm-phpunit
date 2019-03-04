<?hh

namespace Zynga\PHPUnit\V2\Tests\Mock;

use Zynga\PHPUnit\V2\TestCase;
use \Exception;

class ExceptionInAssertPreConditions extends TestCase {
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
    throw new Exception();
  }

  public function testSomething() {
    $this->testSomething = true;
  }

  public function assertPostConditions() {
    $this->assertPostConditions = true;
  }

  public function tearDown() {
    $this->tearDown = true;
  }
}
