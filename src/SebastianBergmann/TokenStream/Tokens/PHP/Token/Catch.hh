<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Tokens\Family\TryTokens;
use SebastianBergmann\TokenStream\Token\Types;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Open_Curly;

class PHP_Token_Catch extends TryTokens {
  private int $endOfDefinitionId = -1;
  private bool $didEndOfDefinitionId = false;

  public function getShortTokenName(): string {
    return 'Catch';
  }

  public function getEndOfDefinitionTokenId(): int {
    if ($this->didEndOfDefinitionId === true) {
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
