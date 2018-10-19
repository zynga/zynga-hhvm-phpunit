<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenWithScope;
use SebastianBergmann\TokenStream\Token\Types;

// Tokens introduced in PHP 5.5
class PHP_Token_Finally extends TokenWithScope {

  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }

  public function getShortTokenName(): string {
    return 'Finally';
  }
  
  public function getEndOfDefinitionLineNo(): int {
    return $this->getLine();
  }

}
