<?hh

namespace Zynga\PHPUnit\V2\Tests\Mock;

use \PHPUnit_Framework_TestCase;

class Failure extends PHPUnit_Framework_TestCase {
  protected function runTest() {
    $this->fail();
  }
}
