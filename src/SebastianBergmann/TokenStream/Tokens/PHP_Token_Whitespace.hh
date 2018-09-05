<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\Token\Types;

final class PHP_Token_Whitespace extends Token {
  final public function getTokenType(): string {
    return Types::T_DEFAULT;
  }
}
