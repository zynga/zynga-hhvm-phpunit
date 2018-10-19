<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenWithScope;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Elseif extends TokenWithScope {

  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }

  public function getShortTokenName(): string {
    return 'Elseif';
  }

}
