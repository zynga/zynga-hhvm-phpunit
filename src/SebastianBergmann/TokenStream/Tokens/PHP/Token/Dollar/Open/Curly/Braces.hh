<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Dollar_Open_Curly_Braces extends Token {

  public function getTokenType(): string {
    return Types::T_DEFAULT;
  }

  public function getShortTokenName(): string {
    return 'Dollar_Open_Curly_Braces';
  }

}
