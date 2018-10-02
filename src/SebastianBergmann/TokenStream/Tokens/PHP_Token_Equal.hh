<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenOperator;

class PHP_Token_Equal extends TokenOperator {

  public function getShortTokenName(): string {
    return 'Equal';
  }

}
