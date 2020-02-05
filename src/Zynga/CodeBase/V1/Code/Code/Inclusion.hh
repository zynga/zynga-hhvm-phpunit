<?hh // strict

namespace Zynga\CodeBase\V1\Code;

class Code_Inclusion {
  private Map<string, bool> $_files;

  public function __construct() {
    $this->_files = Map {};
  }

  public function add(string $file): bool {
    $this->_files->set($file, true);
    return true;
  }

  public function getAll(): Vector<string> {
    return $this->_files->keys();
  }

}
