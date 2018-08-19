<?hh // strict

namespace Zynga\CodeBase\V1\File;

class ExecutableRange {
  private string $_reason;
  private int $_start;
  private int $_end;

  public function __construct(string $reason, int $start, int $end) {
    $this->_reason = $reason;
    $this->_start = $start;
    $this->_end = $end;
  }

  public function getReason(): string {
    return $this->_reason;
  }

  public function getStart(): int {
    return $this->_start;
  }

  public function getEnd(): int {
    return $this->_end;
  }

}
