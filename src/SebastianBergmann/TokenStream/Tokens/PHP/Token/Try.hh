<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Tokens\Family\TryTokens;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Try extends TryTokens {

  public function TryTokens(): string {
    return Types::T_KEYWORD;
  }

  public function getShortTokenName(): string {
    return 'Try';
  }

}
