<?hh // strict

namespace Zynga\CodeBase\V1\File;

use Zynga\CodeBase\V1\File;

class Source {
  private File $_parent;
  private bool $_isLoaded;
  private string $_source;

  public function __construct(File $parent) {
    $this->_parent = $parent;
    $this->_source = '';
    $this->_isLoaded = false;
  }

  public function load(): bool {

    if ($this->_isLoaded === true) {
      return true;
    }

    $fileName = $this->_parent->getFile();

    if (!is_file($fileName) || !is_readable($fileName)) {
      return false;
    }

    $this->_source = file_get_contents($fileName);

    return true;

  }

  public function get(): string {
    $this->load();
    return $this->_source;
  }

}
