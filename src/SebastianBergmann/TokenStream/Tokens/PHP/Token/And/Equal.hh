<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenOperator;

class PHP_Token_And_Equal extends TokenOperator {

  public function getShortTokenName(): string {
    return 'And_Equal';
  }

}
