<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Double_Colon extends Token {
  public function getTokenType(): string {
    return Types::T_DEFAULT;
  }
}
