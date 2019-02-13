<?hh

namespace Zynga\PHPUnit\V2\Tests\Mock;

use \PHPUnit_Framework_TestCase;
use \RuntimeException;

class ThrowExceptionTestCase extends PHPUnit_Framework_TestCase {
  public function test() {
    throw new RuntimeException('A runtime error occurred');
  }
}
