<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\Token\Types;

// Tokens introduced in HackLang / HHVM
class PHP_Token_Async extends Token {
  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }
}
