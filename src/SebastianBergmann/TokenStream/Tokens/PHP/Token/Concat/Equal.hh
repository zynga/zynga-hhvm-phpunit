<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenOperator;

class PHP_Token_Concat_Equal extends TokenOperator {

  public function getShortTokenName(): string {
    return 'Concat_Equal';
  }

}
