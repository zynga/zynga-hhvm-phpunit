<?hh // strict

namespace SebastianBergmann\PHPUnit\Interfaces;

interface ConstraintInterface {
  public function evaluate(
    mixed $other,
    string $description = '',
    bool $doThrowException = false,
  ): bool;
  public function matches(mixed $other): bool;
  public function toString(): string;
  public function failureDescription(mixed $other): string;
  public function setExpected(mixed $expected): bool;
  public function resetExpected(): bool;
  public function count(): int;
}
