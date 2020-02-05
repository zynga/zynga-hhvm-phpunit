<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenWithScopeAndVisibility;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Enum extends TokenWithScopeAndVisibility {

  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }

  public function getShortTokenName(): string {
    return 'Enum';
  }

}
