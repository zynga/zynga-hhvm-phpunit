<?hh // strict

namespace Zynga\CodeBase\V1\Code;

use SebastianBergmann\CodeCoverage\Driver;
use SebastianBergmann\CodeCoverage\ProcessedFile\FileContainer;

use Zynga\CodeBase\V1\FileFactory;
use Zynga\CodeBase\V1\Code\Code_Base;

class Code_Method extends Code_Base {
  public string $methodName = '';
  public string $visibility = '';
  public string $signature = '';

  private int $_linesExecuted = -1;
  private int $_linesExecutable = -1;

  public function getExecutableLines(): int {
    $this->_calculateStats();
    return $this->_linesExecutable;
  }

  public function getExecutedLines(): int {
    $this->_calculateStats();
    return $this->_linesExecuted;
  }

  private function _calculateStats(): void {

    if ($this->_linesExecuted != -1) {
      return;
    }

    $this->_linesExecuted = 0;
    $this->_linesExecutable = 0;

    $processedFile = FileFactory::get($this->file);
    for ($lineNo = $this->startLine; $lineNo < $this->endLine; $lineNo++) {
      $lineState = $processedFile->lineExecutionState()->get($lineNo);
      if ($lineState == Driver::LINE_EXECUTED ||
          $lineState == Driver::LINE_NOT_EXECUTED) {
        $this->_linesExecutable++;
      }
      if ($lineState == Driver::LINE_EXECUTED) {
        $this->_linesExecuted++;
      }
    }

  }

  public function calculateCoverage(): void {

    $this->_calculateStats();

    if ($this->coverage != -1.0) {
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
