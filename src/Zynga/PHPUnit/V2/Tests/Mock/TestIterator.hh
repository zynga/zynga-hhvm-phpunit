<?hh // strict

namespace Zynga\PHPUnit\V2\Tests\Mock;

class TestIterator implements Iterator<mixed> {
  protected Vector<mixed> $array;
  protected int $position = 0;

  public function __construct(mixed $array = null) {
    $this->array = Vector {};

    if (is_array($array)) {
      $this->array->addAll($array);
    }

  }

  public function rewind(): void {
    $this->position = 0;
  }

  public function valid(): bool {
    return $this->position < count($this->array);
  }

  public function key(): int {
    return $this->position;
  }

  public function current(): mixed {
    return $this->array[$this->position];
  }

  public function next(): void {
    $this->position++;
  }
}
