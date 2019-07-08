<?hh // strict

namespace Zynga\PHPUnit\V2\Filter;

use \ReflectionClass;

class FilterEntry {
  private ReflectionClass $_reflection;
  private Vector<mixed> $_args;

  public function __construct(
    ReflectionClass $reflection,
    Vector<mixed> $args,
  ) {
    $this->_reflection = $reflection;
    $this->_args = $args;
  }

  public function getReflection(): ReflectionClass {
    return $this->_reflection;
  }

  public function getArgs(): Vector<mixed> {
    return $this->_args;
  }

}
