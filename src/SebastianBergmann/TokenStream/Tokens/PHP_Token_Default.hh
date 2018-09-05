<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Default extends Token {

  public function getTokenType(): string {
    return Types::T_DEFAULT;
  }

}
