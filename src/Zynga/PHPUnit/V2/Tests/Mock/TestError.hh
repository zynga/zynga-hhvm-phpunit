<?hh

namespace Zynga\PHPUnit\V2\Tests\Mock;

use Zynga\PHPUnit\V2\TestCase;
use \Exception;

class TestError extends TestCase {
  public function testError() {
    throw new Exception('Mock-Test-Error');
  }
}
