<?hh

namespace PHPUnit\Tests\Mock;

use \PHPUnit_Framework_BaseTestListener;
use \PHPUnit_Framework_Test;

class BaseTestListenerSample extends PHPUnit_Framework_BaseTestListener
{
    public $endCount = 0;

    public function endTest(PHPUnit_Framework_Test $test, $time)
    {
        $this->endCount++;
    }
}
