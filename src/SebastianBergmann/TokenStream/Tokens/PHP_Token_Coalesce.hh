<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\Token\Types;

// Tokens introduced in PHP 7.0
class PHP_Token_Coalesce extends Token {

  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }

}
