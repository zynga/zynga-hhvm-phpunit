<?hh

namespace Zynga\PHPUnit\V2\Tests\Mock;

use Zynga\PHPUnit\V2\TestCase;

class ChangeCurrentWorkingDirectory extends TestCase {
  public function testSomethingThatChangesTheCwd() {
    chdir('../');
    $this->assertTrue(true);
  }
}
