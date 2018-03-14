<?hh // strict

namespace SebastianBergmann\CodeCoverage\Driver\HHVM\CodeBlock;

use SebastianBergmann\CodeCoverage\Driver\HHVM\CodeBlock\Base;
use SebastianBergmann\CodeCoverage\Driver\HHVM\LineStack;

class FunctionBlock extends Base {
  private int $_depth;

  public function __construct() {
    parent::__construct();
    $this->_depth = 0;
  }

  public function isStartOfBlock(LineStack $stack, int $lineno): bool {

    // we are already in a block.
    if ( $this->getInBlock() === true ) {
      return false;
    }

    // Detect if it's a multi-line abstract function.
    if ( $stack->hasFunction() === true && $stack->isAbstractFunction() !== true ) {
      $this->setInBlock(true);
      $this->setStartBlock($lineno);
      if ( $stack->doesContainStartingCurly() ) {
        $this->_depth++;
      }
    }

    // Return the detection result.
    return $this->getInBlock();

  }

  public function isEndOfBlock(LineStack $stack, int $lineno): bool {
    if ( $this->getInBlock() === true && $stack->doesContainEndingCurly() === true ) {
      $this->_depth--;
    }
    if ( $this->_depth == 0 ) {
      $this->setInBlock(false);
      $this->setEndBlock($lineno);
      return true;
    }
    return false;
  }

}
