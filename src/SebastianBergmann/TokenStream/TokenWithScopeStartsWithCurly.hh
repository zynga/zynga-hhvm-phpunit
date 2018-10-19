<?hh // strict 

namespace SebastianBergmann\TokenStream;

use SebastianBergmann\TokenStream\TokenWithScope;
use SebastianBergmann\TokenStream\TokenInterface;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Open_Curly;

abstract class TokenWithScopeStartsWithCurly extends TokenWithScope {

  private int $endOfDefinitionId = -1;
  private bool $didEndOfDefinitionId = false;

  public function getEndOfDefinitionLineNo(): int {
    $token = $this->getEndofDefinitionToken();
    if ($token instanceof TokenInterface) {
      return $token->getLine();
    }
    return $this->getLine();
  }

  public function getEndofDefinitionToken(): ?TokenInterface {

    $tokens = $this->tokenStream()->tokens();

    // don't bother running this if we are still at initialization state.
    if ($this->endOfDefinitionId != -1) {
      $endOfDefinitionToken = $tokens->get($this->endOfDefinitionId);

      if ($endOfDefinitionToken instanceof TokenInterface) {
        return $endOfDefinitionToken;
      }
    }

    // echo "skipAmount name=".$this->getName()."\n";

    for ($i = $this->getId(); $i < $tokens->count(); $i++) {

      $token = $tokens->get($i);

      if ($token instanceof PHP_Token_Open_Curly) {
        $this->endOfDefinitionId = $token->getId();
        break;
      }

    }

    $endOfDefinitionToken = $tokens->get($this->endOfDefinitionId);

    return $endOfDefinitionToken;

  }

}
