<?hh // strict

namespace SebastianBergmann\CodeCoverage\Driver\HHVM\CodeBlock;

use SebastianBergmann\CodeCoverage\Driver\HHVM\CodeBlock\Base;
use SebastianBergmann\CodeCoverage\Driver\HHVM\LineStack;

class UseBlock extends Base {

  public function isStartOfBlock(LineStack $stack, int $lineno): bool {

    //echo "useBlock detection\n";

    // we are already in a block.
    if ( $this->getInBlock() === true ) {
      //echo "alreadyInBlock-start\n";
      return false;
    }

    // Detect if it's a multi-line use statement
    if ( $stack->isUseStatement() === true ) {
      //echo "foundUseStatement-start\n";
      $this->setInBlock(true);
      $this->setStartBlock($lineno);
    }

    // Return the detection result.
    return $this->getInBlock();

  }

  public function isEndOfBlock(LineStack $stack, int $lineno): bool {
    //echo "getInBlock=" . $this->getInBlock() . "\n";
    //echo "doesContainSemi=" . $stack->doesContainSemiColon() . "\n";

    if ( $this->getInBlock() === true && $stack->doesContainSemiColon() === true ) {
      $this->setInBlock(false);
      $this->setEndBlock($lineno);
      //if ( $this->getStartBlock() != $this->getEndBlock() ) {
      //  echo
      //    "start block: " . $this->getStartBlock() . " -> " .
      //    "end block  : " . $this->getEndBlock() . "\n";
      //}
      //echo "END_USE - Found end of block!\n";
      return true;
    }
    return false;
  }

}
