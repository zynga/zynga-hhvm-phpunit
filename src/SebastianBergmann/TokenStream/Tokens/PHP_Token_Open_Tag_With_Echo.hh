<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Open_Tag_With_Echo extends Token {
  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }
}
