<?hh // strict

namespace Zynga\PHPUnit\V2\Tests\Mock;

class Struct {
  public float $var;

  public function __construct(float $var) {
    $this->var = $var;
  }
}
