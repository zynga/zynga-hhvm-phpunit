<?hh // strict

namespace PHPUnit\Interfaces;

interface ConstraintInterface {
  public function count(): int;
  public function evaluate(mixed $other, string $description = '', bool $returnResult = false): mixed;
  public function failureDescription(mixed $other): string;
  public function toString(): string;
}
