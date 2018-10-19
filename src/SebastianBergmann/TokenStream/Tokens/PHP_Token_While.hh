<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenWithScopeStartsWithCurly;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_While extends TokenWithScopeStartsWithCurly {

  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }

  public function getShortTokenName(): string {
    return 'While';
  }

}
