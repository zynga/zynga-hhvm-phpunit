<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token as PHP_Token;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Ns_Separator extends PHP_Token {

  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }

  public function getShortTokenName(): string {
    return 'Ns_Separator';
  }

}
