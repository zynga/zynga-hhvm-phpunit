<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Open_Bracket extends Token {
  public function getTokenType(): string {
    return Types::T_DEFAULT;
  }
}
