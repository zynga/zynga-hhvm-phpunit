<?hh // strict

namespace SebastianBergmann\CodeCoverage\Driver\HHVM\CodeBlock;

use SebastianBergmann\CodeCoverage\Driver\HHVM\LineStack;

interface CodeBlockInterface {
  public function setInBlock(bool $inBlock): bool;
  public function getInBlock(): bool;
  public function setStartBlock(int $lineno): bool;
  public function getStartBlock(): int;
  public function setEndBlock(int $lineno): bool;
  public function getEndBlock(): int;
  public function isStartOfBlock(LineStack $stack, int $lineno): bool;
  public function isEndOfBlock(LineStack $stack, int $lineno): bool;
}
