<?hh // strict 

namespace SebastianBergmann\TokenStream;

use SebastianBergmann\TokenStream\TokenWithScope;
use SebastianBergmann\TokenStream\TokenInterface;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Open_Curly;

abstract class TokenWithScopeStartsWithCurly extends TokenWithScope {

  private int $endOfDefinitionId = -1;
  private bool $didEndOfDefinitionId = false;

  public function getEndOfDefinitionTokenId(): int {

    if ( $this->didEndOfDefinitionId == true ) {
      return $this->endOfDefinitionId;
    }

    $this->didEndOfDefinitionId = true;

    $tokens = $this->tokenStream()->tokens();

    for ($i = $this->getId(); $i < $tokens->count(); $i++) {

      $token = $tokens->get($i);

      if ($token instanceof PHP_Token_Open_Curly) {
        $this->endOfDefinitionId = $token->getId();
        break;
      }

    }

    return $this->endOfDefinitionId;

  }

}
