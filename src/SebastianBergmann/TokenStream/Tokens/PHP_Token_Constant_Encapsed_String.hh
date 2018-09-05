<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Constant_Encapsed_String extends Token {

  public function getTokenType(): string {
    return Types::T_CONSTANT;
  }

}
