<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Tokens\PHP_Token_Interface;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_String;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_WhiteSpace;

class PHP_Token_Class extends PHP_Token_Interface {

  /**
   * @return string
   */
  public function getName(): string {

    $tokens = $this->tokenStream->tokens();

    $next = $tokens->get($this->id + 1);

    if ($next instanceof PHP_Token_String) {
      return strval($next);
    }

    if ($next instanceof PHP_Token_Whitespace) {
      $nextNext = $tokens->get($this->id + 2);
      if ($nextNext instanceof PHP_Token_String) {
        return strval($nextNext);
      }
    }

    return 'anonymous class';

  }
}
