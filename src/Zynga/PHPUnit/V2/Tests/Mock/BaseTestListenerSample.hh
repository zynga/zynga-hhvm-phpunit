<?hh // strict

namespace Zynga\PHPUnit\V2\Tests\Mock;

use \PHPUnit_Framework_BaseTestListener;
use \PHPUnit_Framework_Test;

class BaseTestListenerSample extends PHPUnit_Framework_BaseTestListener {
  public int $endCount = 0;

  public function endTest(PHPUnit_Framework_Test $test, float $time): void {
    $this->endCount++;
  }
}
