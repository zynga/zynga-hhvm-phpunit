<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

class Count {

  private int $count = 0;

  final public function get(): int {
    return $this->count;
  }

  final public function add(int $amount): bool {
    $this->count += $amount;
    return true;
  }

  final public function increment(int $byHowMuch = 1): bool {
    $this->count += $byHowMuch;
    return true;
  }

  final public function reset(): bool {
    $this->count = 0;
    return true;
  }

}
