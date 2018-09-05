<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token as PHP_Token;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Tuple extends PHP_Token {
  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }
}
