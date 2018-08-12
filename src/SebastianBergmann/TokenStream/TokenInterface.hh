<?hh // strict

namespace SebastianBergmann\TokenStream;

interface TokenInterface {
  public function setText(string $text): bool;
  public function getText(): string;
  public function setLine(int $line): bool;
  public function getLine(): int;
  public function getTokenStreamId(): int;
  public function setTokenStreamId(int $streamId): bool;
  public function getId(): int;
  public function setId(int $id): bool;
  public function getShortTokenName(): string;
  public function __toString(): string;
}
