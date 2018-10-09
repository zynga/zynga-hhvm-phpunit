<?hh // strict

namespace SebastianBergmann\TokenStream;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\TokenInterface;
use SebastianBergmann\TokenStream\Token\Types;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Semicolon;

abstract class TokenEndsWithSemicolon extends Token {
  private int $endOfDefinitionId = -1;
  private bool $didEndofDefinitionId = false;

  public function getEndOfDefinitionLineNo(): int {
    $token = $this->getEndofDefinitionToken();
    if ( $token instanceof TokenInterface ) {
      return $token->getLine();
    }
    return $this->getLine();
  }

  public function getEndofDefinitionToken(): ?TokenInterface {

    $tokens = $this->tokenStream()->tokens();

    if ( $this->endOfDefinitionId != -1 ) {
      $endOfDefinitionToken = $tokens->get($this->endOfDefinitionId);
      if ( $endOfDefinitionToken instanceof TokenInterface ) {
        return $endOfDefinitionToken;
      }
    }

    $tokenCount = $tokens->count(); 
    $endWasFound = false;
    for ( $i = $this->getId(); $i < $tokenCount; $i++ ) {
      $token = $tokens->get($i);

      if ( ! $token instanceof TokenInterface ) {
        break;
      }

      if ( $token instanceof PHP_Token_Semicolon ) {
        $this->endOfDefinitionId = $token->getId();
        $endWasFound = true;
        break;
      }

    }

    if ( $endWasFound !== true ) {
      $this->endOfDefinitionId = $this->getId();
    }

    $endOfDefinitionToken = $tokens->get($this->endOfDefinitionId);

    return $endOfDefinitionToken;

  }

}
