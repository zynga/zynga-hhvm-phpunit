<?hh // strict

namespace PHPUnit\Tests\Mock;

class Struct {
  public float $var;

  public function __construct(float $var) {
    $this->var = $var;
  }
}
