<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenWithScope;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Ns_Seperator;
use SebastianBergmann\TokenStream\Token\Types;

class PHP_Token_Namespace extends TokenWithScope {

  /**
   * @return string
   */
  public function getName(): string {

    $id = $this->getId();

    $tokens = $this->tokenStream()->tokens();

    $namespace = (string) $tokens[$id + 2];

    for ($i = $id + 3; ; $i += 2) {
      if ($tokens->containsKey($i) &&
          $tokens[$i] instanceof PHP_Token_Ns_Separator) {
        $namespace .= '\\'.$tokens[$i + 1];
      } else {
        break;
      }
    }

    return $namespace;
  }

  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }

}
