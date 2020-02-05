<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenOperator;

class PHP_Token_Xor_Equal extends TokenOperator {

  final public function getShortTokenName(): string {
    return 'Xor_Equal';
  }

}
