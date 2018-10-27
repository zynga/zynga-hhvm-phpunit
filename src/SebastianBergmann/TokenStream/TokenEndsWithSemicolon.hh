<?hh // strict

namespace SebastianBergmann\TokenStream;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\TokenInterface;
use SebastianBergmann\TokenStream\Token\Types;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Semicolon;

abstract class TokenEndsWithSemicolon extends Token {
  private int $endOfDefinitionId = -1;
  private bool $didEndofDefinitionId = false;

  public function getEndTokenId(): int {
    return $this->getEndOfDefinitionTokenId();
  }

  public function getEndOfDefinitionTokenId(): int {

    if ( $this->didEndofDefinitionId === true ) {
      return $this->endOfDefinitionId;
    }

    $this->didEndofDefinitionId = true;

    $tokens = $this->tokenStream()->tokens();

    $tokenCount = $tokens->count();

    for ( $i = $this->getId(); $i < $tokenCount; $i++ ) {
      $token = $tokens->get($i);

      if ( ! $token instanceof TokenInterface ) {
        break;
      }

      if ( $token instanceof PHP_Token_Semicolon ) {
        $this->endOfDefinitionId = $token->getId();
        break;
      }

    }

    return $this->endOfDefinitionId;

  }

}
