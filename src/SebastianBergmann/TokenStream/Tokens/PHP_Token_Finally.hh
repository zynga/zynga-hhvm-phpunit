<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\Token\Types;

// Tokens introduced in PHP 5.5
class PHP_Token_Finally extends Token {
  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }
}
