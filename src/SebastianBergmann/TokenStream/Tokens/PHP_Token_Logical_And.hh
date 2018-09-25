<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Logical_And extends Token {

  public function getTokenType(): string {
    return Types::T_OPERATOR;
  }

  public function getShortTokenName(): string {
    return 'Logical_And';
  }

}
