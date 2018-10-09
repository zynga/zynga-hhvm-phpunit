<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenOperator;

class PHP_Token_Pow_Equal extends TokenOperator {

  public function getShortTokenName(): string {
    return 'Pow_Equal';
  }

}
