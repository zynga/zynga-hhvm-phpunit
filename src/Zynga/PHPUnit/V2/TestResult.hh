<?hh // strict

namespace Zynga\PHPUnit\V2;

use Zynga\PHPUnit\V2\Interfaces\TestListenerInterface;
use Zynga\PHPUnit\V2\TestResult\Listeners;

class TestResult {
  private Listeners $_listeners;

  public function __construct() {
    $this->_listeners = new Listeners();
  }

  public function listeners(): Listeners {
    return $this->_listeners;
  }

  public function addListener(TestListenerInterface $listener): void {
    $this->_listeners->add($listener);
  }

  public function removeListener(TestListenerInterface $listener): void {
    $this->_listeners->remove($listener);
  }

  public function flushListeners(): void {
    $this->_listeners->flush();
  }

}
