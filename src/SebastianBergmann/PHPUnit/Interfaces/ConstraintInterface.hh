<?hh // strict

namespace SebastianBergmann\PHPUnit\Interfaces;

interface ConstraintInterface {
  public function evaluate(mixed $other, string $description = '', bool $doThrowException = false): bool;
  public function toString(): string;
}
