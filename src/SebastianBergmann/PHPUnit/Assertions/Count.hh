<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

class Count {

  private int $count = 0;

  public function get(): int {
    return $this->count;
  }

  public function add(int $amount): bool {
    $this->count += $amount;
    return true;
  }

  public function increment(int $byHowMuch = 1): bool {
    $this->count += $byHowMuch;
    return true;
  }

  public function reset(): bool {
    $this->count = 0;
    return true;
  }

}
