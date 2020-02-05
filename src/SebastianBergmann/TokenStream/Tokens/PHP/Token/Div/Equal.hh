<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenOperator;

class PHP_Token_Div_Equal extends TokenOperator {

  public function getShortTokenName(): string {
    return 'Div_Equal';
  }

}
