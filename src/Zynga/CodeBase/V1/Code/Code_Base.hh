<?hh // strict

namespace Zynga\CodeBase\V1\Code;

abstract class Code_Base {
  public string $file;
  public float $coverage;

  private int $_ccn;
  private float $_crap;

  public string $link;
  public int $startLine;
  public int $endLine;
  public string $keywords;
  public string $docblock;

  public function __construct() {
    $this->file = '';
    $this->coverage = -1.0;
    $this->_ccn = -1;
    $this->_crap = -1.0;
    $this->link = '';
    $this->startLine = -1;
    $this->endLine = -1;
    $this->keywords = '';
    $this->docblock = '';
  }

  abstract public function getExecutableLines(): int;
  abstract public function getExecutedLines(): int;
  abstract public function calculateCoverage(): void;

  public function getCcn(): int {
    return $this->_ccn;
  }

  public function setCcn(int $ccn): void {
    $this->_ccn = $ccn;
  }

  public function getCrap(): float {
    return $this->_crap;
  }

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

    if ($this->_crap != -1) {
      return;
    }

    $ccn = $this->getCcn();
    $coverage = $this->coverage;

    // For some reason this item's ccn has never been calculated or accounted
    // for, ignore it.
    if ($ccn == -1) {
      return;
    }

    if ($coverage == 0) {
      $this->_crap = floatval(pow($ccn, 2) + $ccn);
      return;
    }

    if ($coverage >= 95) {
      $this->_crap = floatval($ccn);
      return;
    }

    $this->_crap = pow($ccn, 2) * pow(1 - $coverage / 100, 3) + $ccn;

  }

}
