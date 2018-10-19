<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenWithScope;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Catch extends TokenWithScope {

  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }

  public function getShortTokenName(): string {
    return 'Catch';
  }

  public function getEndOfDefinitionLineNo(): int {
    return $this->getLine();
  }

}
