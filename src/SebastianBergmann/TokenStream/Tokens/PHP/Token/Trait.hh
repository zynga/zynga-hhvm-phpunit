<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Tokens\PHP_Token_Interface;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Trait extends PHP_Token_Interface {
  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }
}
