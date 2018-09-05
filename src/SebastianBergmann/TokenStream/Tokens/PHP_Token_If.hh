<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_If extends Token {
  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }
}
