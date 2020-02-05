<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Unset_Cast extends Token {

  public function getTokenType(): string {
    return Types::T_OPERATOR;
  }

  public function getShortTokenName(): string {
    return 'Unset_Cast';
  }

}
