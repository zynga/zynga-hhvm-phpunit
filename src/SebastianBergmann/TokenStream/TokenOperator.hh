<?hh // strict 

namespace SebastianBergmann\TokenStream;

use SebastianBergmann\TokenStream\TokenEndsWithSemicolon;
use SebastianBergmann\TokenStream\Token\Types;

abstract class TokenOperator extends TokenEndsWithSemicolon {

  public function getTokenType(): string {
    return Types::T_OPERATOR;
  }

}
