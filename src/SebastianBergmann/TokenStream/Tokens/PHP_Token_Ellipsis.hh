<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\Token\Types;

// Tokens introduced in PHP 5.6
class PHP_Token_Ellipsis extends Token {

  public function getTokenType(): string {
    return Types::T_OPERATOR;
  }

}
