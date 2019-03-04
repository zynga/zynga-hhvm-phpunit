<?hh // strict

namespace Zynga\PHPUnit\V2\Tests\Mock;

use Zynga\PHPUnit\V2\TestCase;
use Zynga\PHPUnit\V2\Interfaces\TestInterface;

use \PHPUnit_Framework_BaseTestListener;

class BaseTestListenerSample extends PHPUnit_Framework_BaseTestListener {
  public int $endCount = 0;

  public function endTest(TestInterface $test, float $time): void {
    $this->endCount++;
  }
}
