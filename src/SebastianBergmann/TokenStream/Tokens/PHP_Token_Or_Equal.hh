<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenOperator;

class PHP_Token_Or_Equal extends TokenOperator {

  public function getShortTokenName(): string {
    return 'Or_Equal';
  }

}
