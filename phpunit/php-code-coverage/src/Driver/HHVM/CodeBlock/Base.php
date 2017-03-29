<?hh // strict

namespace SebastianBergmann\CodeCoverage\Driver\HHVM\CodeBlock;

use SebastianBergmann\CodeCoverage\Driver\HHVM\LineStack;

abstract class Base {
  private bool $_inBlock;
  private int $_startBlock;
  private int $_endBlock;

  public function __construct() {
    $this->_inBlock = false;
    $this->_startBlock = -1;
    $this->_endBlock = -1;
  }

  public function setInBlock(bool $inBlock): bool {
    $this->_inBlock = $inBlock;
    return true;
  }

  public function getInBlock(): bool {
    return $this->_inBlock;
  }

  public function setStartBlock(int $lineno): bool {
    $this->_startBlock = $lineno;
    return true;
  }

  public function getStartBlock(): int {
    return $this->_startBlock;
  }

  public function setEndBlock(int $lineno): bool {
    $this->_endBlock = $lineno;
    return true;
  }

  public function getEndBlock(): int {
    return $this->_endBlock;
  }

  abstract public function isStartOfBlock(LineStack $stack, int $lineno): bool;

  abstract public function isEndOfBlock(LineStack $stack, int $lineno): bool;

}
