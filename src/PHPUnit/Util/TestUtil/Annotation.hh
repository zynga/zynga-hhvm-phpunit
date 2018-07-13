<?hh // strict

namespace PHPUnit\Util\TestUtil;

class Annotation {
  public string $name;
  public string $value;

  public function __construct(string $name, string $value) {
    $this->name = $name;
    $this->value = $value;
  }

}
