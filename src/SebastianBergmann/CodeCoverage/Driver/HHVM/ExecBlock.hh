<?hh // strict

namespace SebastianBergmann\CodeCoverage\Driver\HHVM;

class ExecBlock {
  private int $start;
  private int $stop;

  public function __construct(int $start = 0, int $stop = 0) {
    $this->start = $start;
    $this->stop = $stop;
  }

  public function getStart(): int {
    return $this->start;
  }

  public function getStop(): int {
    return $this->stop;
  }

}
