<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenWithScope;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Try extends TokenWithScope {

  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }

  public function getShortTokenName(): string {
    return 'Try';
  }

  public function getEndOfDefinitionLineNo(): int {
    return $this->getLine();
  }

}
