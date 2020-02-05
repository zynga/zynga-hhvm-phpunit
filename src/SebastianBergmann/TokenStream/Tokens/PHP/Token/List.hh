<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenEndsWithSemicolon;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_List extends TokenEndsWithSemicolon {

  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }

  public function getShortTokenName(): string {
    return 'List';
  }

}
