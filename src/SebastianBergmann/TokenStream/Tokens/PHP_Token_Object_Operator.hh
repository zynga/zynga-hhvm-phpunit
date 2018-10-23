<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenEndsWithSemicolon;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Object_Operator extends TokenEndsWithSemicolon {

  public function getTokenType(): string {
    return Types::T_OPERATOR;
  }

  public function getShortTokenName(): string {
    return 'Object_Operator';
  }

  /*
  public function getEndLineNo(): int {
    return $this->getEndOfDefinitionLineNo();
  }
  */

}
