<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Close_Square extends Token {

  public function getTokenType(): string {
    return Types::T_DEFAULT;
  }

}
