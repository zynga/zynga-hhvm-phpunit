<?hh // strict

namespace Zynga\PHPUnit\V2\TestResult;

use Zynga\PHPUnit\V2\Interfaces\TestListenerInterface;
use \PHPUnit_Util_Printer;
use \PHPUnit_Framework_Test;
use \PHPUnit_Framework_TestSuite;
use \Exception;

class Listeners implements TestListenerInterface {
  private Vector<TestListenerInterface> $_listeners = Vector {};

  final public function add(TestListenerInterface $listener): void {
    $this->_listeners->add($listener);
  }

  final public function remove(TestListenerInterface $listener): void {

    $offset = 0;
    $found = false;
    foreach ($this->_listeners as $_listener) {
      if ($listener === $_listener) {
        $found = true;
        break;
      }
      $offset++;
    }
    if ($found === true) {
      $this->_listeners->removeKey($offset);
    }
  }

  final public function flush(): void {
    foreach ($this->_listeners as $listener) {
      $listener->flush();
    }
  }

  final public function addError(
    PHPUnit_Framework_Test $test,
    Exception $e,
    float $time,
  ): void {
    foreach ($this->_listeners as $listener) {
      $listener->addError($test, $e, $time);
    }
  }

  /**
   * A warning occurred.
   *
   * @param PHPUnit_Framework_Test    $test
   * @param PHPUnit_Framework_Warning $e
   * @param float                     $time
   *
   * @since Method available since Release 6.0.0
   */
  final public function addWarning(
    PHPUnit_Framework_Test $test,
    Exception $e,
    float $time,
  ): void {
    foreach ($this->_listeners as $listener) {
      $listener->addWarning($test, $e, $time);
    }
  }

  final public function addFailure(
    PHPUnit_Framework_Test $test,
    Exception $e,
    float $time,
  ): void {
    foreach ($this->_listeners as $listener) {
      $listener->addFailure($test, $e, $time);
    }
  }

  final public function addIncompleteTest(
    PHPUnit_Framework_Test $test,
    Exception $e,
    float $time,
  ): void {
    foreach ($this->_listeners as $listener) {
      $listener->addIncompleteTest($test, $e, $time);
    }
  }

  final public function addRiskyTest(
    PHPUnit_Framework_Test $test,
    Exception $e,
    float $time,
  ): void {
    foreach ($this->_listeners as $listener) {
      $listener->addRiskyTest($test, $e, $time);
    }
  }

  final public function addSkippedTest(
    PHPUnit_Framework_Test $test,
    Exception $e,
    float $time,
  ): void {
    foreach ($this->_listeners as $listener) {
      $listener->addSkippedTest($test, $e, $time);
    }
  }

  final public function startTestSuite(
    PHPUnit_Framework_TestSuite $suite,
  ): void {
    foreach ($this->_listeners as $listener) {
      $listener->startTestSuite($suite);
    }
  }

  final public function endTestSuite(
    PHPUnit_Framework_TestSuite $suite,
  ): void {
    foreach ($this->_listeners as $listener) {
      $listener->endTestSuite($suite);
    }
  }

  final public function startTest(PHPUnit_Framework_Test $test): void {
    foreach ($this->_listeners as $listener) {
      $listener->startTest($test);
    }
  }

  final public function endTest(
    PHPUnit_Framework_Test $test,
    float $time,
  ): void {
    foreach ($this->_listeners as $listener) {
      $listener->endTest($test, $time);
    }
  }

}
