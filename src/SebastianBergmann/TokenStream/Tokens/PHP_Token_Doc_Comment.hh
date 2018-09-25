<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Doc_Comment extends Token {

  public function getTokenType(): string {
    return Types::T_COMMENT;
  }

  public function getShortTokenName(): string {
    return 'Doc_Comment';
  }

}
