<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenOperator;

class PHP_Token_Mod_Equal extends TokenOperator {

  public function getShortTokenName(): string {
    return 'Mod_Equal';
  }

}
