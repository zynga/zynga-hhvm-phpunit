<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Tokens\PHP_Token_Includes;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Require_Once extends PHP_Token_Includes {

  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }

  public function getShortTokenName(): string {
    return 'Require_Once';
  }

}
