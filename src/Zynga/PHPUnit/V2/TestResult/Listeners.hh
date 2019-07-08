<?hh // strict

namespace Zynga\PHPUnit\V2\TestResult;

use Zynga\PHPUnit\V2\Interfaces\TestInterface;
use Zynga\PHPUnit\V2\Interfaces\TestListenerInterface;
use Zynga\PHPUnit\V2\TestCase;

use \PHPUnit_Util_Printer;
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
    TestInterface $test,
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
   * @param TestInterface    $test
   * @param Exception $e
   * @param float                     $time
   *
   * @since Method available since Release 6.0.0
   */
  final public function addWarning(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {
    foreach ($this->_listeners as $listener) {
      $listener->addWarning($test, $e, $time);
    }
  }

  final public function addFailure(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {
    foreach ($this->_listeners as $listener) {
      $listener->addFailure($test, $e, $time);
    }
  }

  final public function addIncompleteTest(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {
    foreach ($this->_listeners as $listener) {
      $listener->addIncompleteTest($test, $e, $time);
    }
  }

  final public function addRiskyTest(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {
    foreach ($this->_listeners as $listener) {
      $listener->addRiskyTest($test, $e, $time);
    }
  }

  final public function addSkippedTest(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {
    foreach ($this->_listeners as $listener) {
      $listener->addSkippedTest($test, $e, $time);
    }
  }

  final public function startTestSuite(TestInterface $suite): void {
    foreach ($this->_listeners as $listener) {
      $listener->startTestSuite($suite);
    }
  }

  final public function endTestSuite(TestInterface $suite): void {
    foreach ($this->_listeners as $listener) {
      $listener->endTestSuite($suite);
    }
  }

  final public function startTest(TestInterface $test): void {
    foreach ($this->_listeners as $listener) {
      $listener->startTest($test);
    }
  }

  final public function endTest(TestInterface $test, float $time): void {
    foreach ($this->_listeners as $listener) {
      $listener->endTest($test, $time);
    }
  }

}
