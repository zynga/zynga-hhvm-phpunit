<?hh // strict

namespace SebastianBergmann\CodeCoverage\Driver\HHVM\CodeBlock;

use SebastianBergmann\CodeCoverage\Driver\HHVM\CodeBlock\Base;
use SebastianBergmann\CodeCoverage\Driver\HHVM\LineStack;

class IfBlock extends Base {

  public function isStartOfBlock(LineStack $stack, int $lineno): bool {

    // we are already in a block.
    if ($this->getInBlock() === true) {
      return false;
    }

    // Detect if it's a multi-line if or elseif function.
    if ($stack->doesContainIf() === true &&
        $stack->doesContainSemiColon() === false &&
        $stack->doesContainStartingCurly() === false) {
      $this->setInBlock(true);
      $this->setStartBlock($lineno);
    }

    // Return the detection result.
    return $this->getInBlock();

  }

  public function isEndOfBlock(LineStack $stack, int $lineno): bool {
    if ($this->getInBlock() === true &&
        $stack->doesContainEndingCurly() === true) {
      $this->setInBlock(false);
      $this->setEndBlock($lineno);
      return true;
    }
    return false;
  }

}
