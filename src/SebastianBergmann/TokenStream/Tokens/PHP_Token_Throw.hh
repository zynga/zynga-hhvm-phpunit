<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenEndsWithSemicolon;
use SebastianBergmann\TokenStream\TokenInterface;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Throw extends TokenEndsWithSemicolon {

  final public function getTokenType(): string {
    return Types::T_KEYWORD;
  }

  final public function getShortTokenName(): string {
    return 'Throw';
  }

}
