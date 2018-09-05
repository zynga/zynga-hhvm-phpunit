<?hh

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token as PHP_Token;
use SebastianBergmann\TokenStream\Token\Types;

// Tokens introduced in PHP 5.1
class PHP_Token_Halt_Compiler extends PHP_Token {
  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }
}
