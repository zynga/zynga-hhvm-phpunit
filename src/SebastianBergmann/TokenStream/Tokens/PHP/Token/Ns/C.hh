<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Ns_C extends Token {

  public function getTokenType(): string {
    return Types::T_DEFAULT;
  }

  public function getShortTokenName(): string {
    return 'Ns_C';
  }

}
