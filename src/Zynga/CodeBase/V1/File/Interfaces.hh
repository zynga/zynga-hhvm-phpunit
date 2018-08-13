<?hh // strict

namespace Zynga\CodeBase\V1\File;

use Zynga\CodeBase\V1\File;
use Zynga\CodeBase\V1\Code\Code_Interface;

class Interfaces {
  private File $_parent;
  private Map<string, Code_Interface> $_interfaces;

  public function __construct(File $parent) {
    $this->_parent = $parent;
    $this->_interfaces = Map {};
  }

  public function add(string $name, Code_Interface $interface): bool {
    $this->_interfaces->set($name, $interface);
    return true;
  }

  public function get(string $name): ?Code_Interface {
    return $this->_interfaces->get($name);
  }

  public function getAll(): Map<string, Code_Interface> {
    return $this->_interfaces;
  }

}
