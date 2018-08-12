<?hh // strict

namespace Zynga\CodeBase\V1\File;

use Zynga\CodeBase\V1\File;
use Zynga\CodeBase\V1\Code\Code_Class;

class Classes {
  private File $_parent;
  private Map<string, Code_Class> $_classes;

  public function __construct(File $parent) {
    $this->_parent = $parent;
    $this->_classes = Map {};
  }

  public function add(string $name, Code_Class $class): bool {
    $this->_classes->set($name, $class);
    return true;
  }

  public function get(string $name): ?Code_Class {
    return $this->_classes->get($name);
  }

  public function getAll(): Map<string, Code_Class> {
    return $this->_classes;
  }

}
