<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Nullsafe_Object_Operator extends Token {
  public function getTokenType(): string {
    return Types::T_OPERATOR;
  }
}
