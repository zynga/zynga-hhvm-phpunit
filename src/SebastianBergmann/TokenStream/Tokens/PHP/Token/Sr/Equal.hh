<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenOperator;

class PHP_Token_Sr_Equal extends TokenOperator {

  public function getShortTokenName(): string {
    return 'Sr_Equal';
  }

}
