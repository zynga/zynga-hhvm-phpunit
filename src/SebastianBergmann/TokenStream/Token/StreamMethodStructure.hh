<?hh // strict

namespace SebastianBergmann\TokenStream\Token;

class StreamMethodStructure extends StreamStructureBase {
  public string $methodName = '';
  public string $visibility = '';
  public string $signature = '';

  public function calculateCoverage(): void {
    if ($this->coverage != -1.0) {
      return;
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
