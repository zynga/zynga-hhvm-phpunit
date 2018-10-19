<?hh // strict

namespace SebastianBergmann\TokenStream;

use Zynga\CodeBase\V1\File;

interface TokenInterface {
  public function setAllAttributes(
    string $text,
    int $line,
    File $file,
    int $id,
  ): void;
  public function getEndOfDefinitionLineNo(): int;
  public function setText(string $text): bool;
  public function getText(): ?string;
  public function setLine(int $line): bool;
  public function getLine(): int;
  public function getEndTokenId(): int;
  public function getEndLine(): int;
  public function getId(): int;
  public function setId(int $id): bool;
  public function getShortTokenName(): string;
  //public function getFile(): ?File;
  //public function setFile(File $file): bool;
  public function getTokenType(): string;
  public function __toString(): string;
}
