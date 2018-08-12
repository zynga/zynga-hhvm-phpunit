<?hh // strict

namespace Zynga\CodeBase\V1\File;

use Zynga\CodeBase\V1\File;
use Zynga\CodeBase\V1\Code\Code_Method;

class Functions {
  private File $_parent;
  private Map<string, Code_Method> $_functions;

  public function __construct(File $parent) {
    $this->_parent = $parent;
    $this->_functions = Map {};
  }

  public function add(string $name, Code_Method $method): bool {
    $this->_functions->set($name, $method);
    return true;
  }

  public function getAll(): Map<string, Code_Method> {
    return $this->_functions;
  }

}
