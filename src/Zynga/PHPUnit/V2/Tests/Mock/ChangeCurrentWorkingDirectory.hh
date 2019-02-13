<?hh

namespace Zynga\PHPUnit\V2\Tests\Mock;

use \PHPUnit_Framework_TestCase;

class ChangeCurrentWorkingDirectory extends PHPUnit_Framework_TestCase {
  public function testSomethingThatChangesTheCwd() {
    chdir('../');
    $this->assertTrue(true);
  }
}
