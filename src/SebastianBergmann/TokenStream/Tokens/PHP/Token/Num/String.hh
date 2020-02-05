<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Num_String extends Token {
  
  public function getTokenType(): string {
    return Types::T_DEFAULT;
  }

  public function getShortTokenName(): string {
    return 'Num_String';
  }

}
