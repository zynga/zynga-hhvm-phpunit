<?hh

namespace Zynga\PHPUnit\V2\Tests\Mock;

use Zynga\PHPUnit\V2\TestCase;
use \RuntimeException;

class ThrowExceptionTestCase extends TestCase {
  public function test() {
    throw new RuntimeException('MOCK: A runtime error occurred');
  }
}
