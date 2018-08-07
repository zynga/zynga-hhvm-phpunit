<?hh // strict

namespace SebastianBergmann\TokenStream\Token;

use SebastianBergmann\TokenStream\Token\StreamMethodStructure;

class StreamClassStructure extends StreamStructureBase {
  public Map<string, StreamMethodStructure> $methods = Map {};
  public int $numMethods = -1;
  public int $numTestedMethods = -1;
  public mixed $parent = false;
  public mixed $interfaces = false;
  public Map<string, string> $package = Map {};
  public string $className = '';

  public function calculateCoverage(): void {

    if ($this->coverage != -1) {
      return;
    }

    foreach ($this->methods as $methodName => $methodObj) {
      $this->executableLines += $methodObj->executableLines;
      $this->executedLines += $methodObj->executedLines;
    }

    if ($this->executableLines > 0) {
      $this->coverage =
        floatval(($this->executedLines / $this->executableLines) * 100);
    } else {
      $this->coverage = 100.0;
    }

    $this->calculateCrap();

  }

}
