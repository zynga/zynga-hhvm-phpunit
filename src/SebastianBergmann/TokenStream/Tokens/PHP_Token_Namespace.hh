<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenWithScope;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Ns_Seperator;

class PHP_Token_Namespace extends TokenWithScope {

  /**
   * @return string
   */
  public function getName(): string {

    $tokens = $this->tokenStream()->tokens();

    $namespace = (string) $tokens[$this->id + 2];

    for ($i = $this->id + 3; ; $i += 2) {
      if ($tokens->containsKey($i) &&
          $tokens[$i] instanceof PHP_Token_Ns_Separator) {
        $namespace .= '\\'.$tokens[$i + 1];
      } else {
        break;
      }
    }

    return $namespace;
  }
}
