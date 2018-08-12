<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenInterface;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Interface;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_String;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_WhiteSpace;

class PHP_Token_Class extends PHP_Token_Interface {

  protected int $endOfDefinitionId = -1;
  protected bool $didEndOfDefinitionId = false;

  public function getEndOfDefinitionLineNo(): int {
    $token = $this->getEndofDefinitionToken();
    if ($token instanceof TokenInterface) {
      return $token->getLine();
    }
    return $this->getLine();
  }

  public function getEndofDefinitionToken(): ?TokenInterface {

    $tokens = $this->tokenStream()->tokens();

    $endOfDefinitionToken = $tokens->get($this->endOfDefinitionId);

    if ($endOfDefinitionToken instanceof TokenInterface) {
      return $endOfDefinitionToken;
    }

    // echo "skipAmount name=".$this->getName()."\n";

    for ($i = $this->getId(); $i < $tokens->count(); $i++) {

      $token = $tokens->get($i);

      if ($token instanceof PHP_Token_Open_Curly) {
        $this->endOfDefinitionId = $token->getId();
        break;
      }

    }

    $endOfDefinitionToken = $tokens->get($this->endOfDefinitionId);

    return $endOfDefinitionToken;

  }

  /**
   * @return string
   */
  public function getName(): string {

    $tokens = $this->tokenStream()->tokens();

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
