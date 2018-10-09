<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\Token\Types;
use SebastianBergmann\TokenStream\TokenInterface;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Open_Curly;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Semicolon;

class PHP_Token_If extends Token {
  private int $endOfDefinitionId = -1;
  private bool $didEndOfDefinitionId = false;

  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }

  public function getShortTokenName(): string {
    return 'If';
  }

  public function getEndOfDefinitionLineNo(): int {
    $token = $this->getEndofDefinitionToken();
    if ($token instanceof TokenInterface) {
      return $token->getLine();
    }
    return $this->getLine();
  }

  public function getEndofDefinitionToken(): ?TokenInterface {

    $tokens = $this->tokenStream()->tokens();

    $endOfDefinitionToken = $tokens->get($this->endOfDefinitionId);

    if ($endOfDefinitionToken instanceof TokenInterface) {
      return $endOfDefinitionToken;
    }

    // echo "skipAmount name=".$this->getName()."\n";

    for ($i = $this->getId(); $i < $tokens->count(); $i++) {

      $token = $tokens->get($i);

      if ($token instanceof PHP_Token_Open_Curly) {
        $this->endOfDefinitionId = $token->getId();
        break;
      }
      
      if ($token instanceof PHP_Token_Semicolon) {
        $this->endOfDefinitionId = $token->getId();
        break;
      }

    }

    $endOfDefinitionToken = $tokens->get($this->endOfDefinitionId);

    return $endOfDefinitionToken;

  }

}
