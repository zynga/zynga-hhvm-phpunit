<?hh // strict

namespace Zynga\CodeBase\V1\File;

use Zynga\CodeBase\V1\File;
use Zynga\CodeBase\V1\Code\Code_Class;

class Traits {
  private File $_parent;
  private Map<string, Code_Class> $_traits;

  public function __construct(File $parent) {
    $this->_parent = $parent;
    $this->_traits = Map {};
  }

  public function add(string $name, Code_Class $trait): bool {
    $this->_traits->set($name, $trait);
    return true;
  }

  public function get(string $name): ?Code_Class {
    return $this->_traits->get($name);
  }

  public function getAll(): Map<string, Code_Class> {
    return $this->_traits;
  }

}
