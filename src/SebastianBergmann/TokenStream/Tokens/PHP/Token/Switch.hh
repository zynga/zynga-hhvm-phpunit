<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Switch extends Token {

  final public function getTokenType(): string {
    return Types::T_KEYWORD;
  }

  final public function getShortTokenName(): string {
    return 'Switch';
  }

}
