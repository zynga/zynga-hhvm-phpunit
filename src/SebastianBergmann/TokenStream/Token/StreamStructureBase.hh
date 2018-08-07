<?hh // strict

namespace SebastianBergmann\TokenStream\Token;

abstract class StreamStructureBase {
  public string $file = '';
  public int $executableLines = -1;
  public int $executedLines = -1;
  public float $coverage = -1.0;
  public int $ccn = -1;
  public float $crap = -1.0;
  public string $link = '';
  public int $startLine = -1;
  public int $endLine = -1;
  public string $keywords = '';
  public string $docblock = '';

  abstract public function calculateCoverage(): void;

  /**
   * Calculates the Change Risk Anti-Patterns (CRAP) index for a unit of code
   * based on its cyclomatic complexity and percentage of code coverage.
   *
   * @param int   $ccn
   * @param float $coverage
   *
   * @return string
   */
  protected function calculateCrap(): void {
    if ($this->coverage == 0) {
      $this->crap = (pow($this->ccn, 2) + $this->ccn);
    }

    if ($this->coverage >= 95) {
      $this->crap = floatval($this->ccn);
    }

    $this->crap =
      pow($this->ccn, 2) * pow(1 - $this->coverage / 100, 3) + $this->ccn;
  }

}
