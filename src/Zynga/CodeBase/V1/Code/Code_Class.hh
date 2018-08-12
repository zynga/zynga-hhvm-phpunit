<?hh // strict

namespace Zynga\CodeBase\V1\Code;

use Zynga\CodeBase\V1\Code\Code_Base;
use Zynga\CodeBase\V1\Code\Code_Method;

class Code_Class extends Code_Base {
  public Map<string, Code_Method> $methods = Map {};

  public int $numMethods = -1;
  public int $numTestedMethods = -1;
  public mixed $parent = false;
  public mixed $interfaces = false;
  public Map<string, string> $package = Map {};
  public string $className = '';

  public function getNumMethods(): int {
    return $this->methods->count();
  }

  public function getExecutableLines(): int {
    $executableLines = 0;
    foreach ($this->methods as $methodName => $methodObj) {
      $executableLines += $methodObj->getExecutableLines();
    }
    return $executableLines;
  }

  public function getExecutedLines(): int {
    $executedLines = 0;
    foreach ($this->methods as $methodName => $methodObj) {
      $executedLines += $methodObj->getExecutedLines();
    }
    return $executedLines;
  }

  public function calculateCoverage(): void {

    if ($this->coverage != -1) {
      return;
    }

    if ($this->getExecutableLines() > 0) {
      $this->coverage = floatval(
        ($this->getExecutedLines() / $this->getExecutableLines()) * 100,
      );
    } else {
      $this->coverage = 100.0;
    }

    $this->calculateCrap();

  }

}
