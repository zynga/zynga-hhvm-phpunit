<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\TokenInterface;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Throw extends Token {
  protected int $endOfDefinitionId = -1;
  protected bool $didEndOfDefinitionId = false;

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

      if ($token instanceof PHP_Token_Semicolon) {
        $this->endOfDefinitionId = $token->getId();
        break;
      }

    }

    $endOfDefinitionToken = $tokens->get($this->endOfDefinitionId);

    return $endOfDefinitionToken;

  }

  final public function getTokenType(): string {
    return Types::T_KEYWORD;
  }

  final public function getShortTokenName(): string {
    return 'Throw';
  }

}
