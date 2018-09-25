<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Xor_Equal extends Token {

  final public function getTokenType(): string {
    return Types::T_OPERATOR;
  }

  final public function getShortTokenName(): string {
    return 'Xor_Equal';
  }

}
