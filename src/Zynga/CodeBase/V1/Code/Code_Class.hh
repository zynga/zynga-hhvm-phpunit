<?hh // strict

namespace Zynga\CodeBase\V1\Code;

use Zynga\CodeBase\V1\Code\Code_Base;
use Zynga\CodeBase\V1\Code\Code_Method;

class Code_Class extends Code_Base {
  public Map<string, Code_Method> $methods = Map {};

  public mixed $parent = false;
  public mixed $interfaces = false;
  public Map<string, string> $package = Map {};
  public string $className = '';

  public function getNumMethods(): int {
    return $this->methods->count();
  }

  public function getNumTestedMethods(): int {
    $numTested = 0;
    foreach ($this->methods as $methodName => $methodObj) {
      $methodObj->calculateCoverage();
      if ($methodObj->coverage == 100) {
        $numTested++;
      }
    }
    return $numTested;
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

    $executableLines = $this->getExecutableLines();
    $executedLines = $this->getExecutedLines();

    if ($executableLines > 0) {
      $this->coverage = floatval(($executedLines / $executableLines) * 100);
    } else {
      $this->coverage = 100.0;
    }

    $this->calculateCrap();

  }

}
