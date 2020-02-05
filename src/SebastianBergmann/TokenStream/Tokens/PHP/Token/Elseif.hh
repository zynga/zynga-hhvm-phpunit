<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Tokens\Family\IfTokens;
use SebastianBergmann\TokenStream\Token\Types;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Open_Curly;

class PHP_Token_Elseif extends IfTokens {
  private int $endOfDefinitionId = -1;
  private bool $didEndOfDefinitionId = false;

  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }

  public function getShortTokenName(): string {
    return 'Elseif';
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
