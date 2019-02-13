<?hh

namespace Zynga\PHPUnit\V2\Tests\Mock;

use \PHPUnit_Framework_TestCase;

class WasRun extends PHPUnit_Framework_TestCase {
  public $wasRun = false;

  protected function runTest() {
    $this->wasRun = true;
  }
}
