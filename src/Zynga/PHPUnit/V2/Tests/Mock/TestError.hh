<?hh

namespace Zynga\PHPUnit\V2\Tests\Mock;

use \PHPUnit_Framework_TestCase;
use \Exception;

class TestError extends PHPUnit_Framework_TestCase {
  protected function runTest() {
    throw new Exception();
  }
}
