<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Plus_Equal extends Token {
  public function getTokenType(): string {
    return Types::T_OPERATOR;
  }
}
