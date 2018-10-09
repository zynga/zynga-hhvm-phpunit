<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenOperator;

class PHP_Token_Sl_Equal extends TokenOperator {

  public function getShortTokenName(): string {
    return 'Sl_Equal';
  }

}
