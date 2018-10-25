<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens\Family;

use SebastianBergmann\TokenStream\TokenWithScope;
use SebastianBergmann\TokenStream\Token\Types;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Else;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Elseif;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Whitespace;

abstract class IfTokens extends TokenWithScope {
  private int $_continuationId = -1;
  private bool $_didContinuationId = false;

  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }

  public function getContinuationTokenId(): int {

    if ($this->_didContinuationId == true) {
      return $this->_continuationId;
    }

    $tokens = $this->tokenStream()->tokens();

    $this->_didContinuationId = true;

    for ($i = $this->getEndTokenId(); $i < $tokens->count(); $i++) {

      $token = $tokens->get($i);

      if ($token instanceof PHP_Token_Whitespace) {
        continue;
      } else if ($token instanceof PHP_Token_Else) {
        $this->_continuationId = $token->getId();
        break;
      } else if ($token instanceof PHP_Token_Elseif) {
        $this->_continuationId = $token->getId();
        break;
      } else {
        // anything else is a exit moment.
        break;
      }

    }

    return $this->_continuationId;

  }

}
