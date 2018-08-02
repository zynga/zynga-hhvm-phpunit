<?hh // strict

namespace SebastianBergmann\TokenStream;

interface TokenInterface {
  public function getLine(): int;
  public function __toString(): string;
}
