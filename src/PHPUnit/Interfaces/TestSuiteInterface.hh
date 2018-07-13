<?hh // strict

namespace PHPUnit\Interfaces;

use \Countable;

interface TestSuiteInterface extends Countable {
  public function getName(): string;
}
