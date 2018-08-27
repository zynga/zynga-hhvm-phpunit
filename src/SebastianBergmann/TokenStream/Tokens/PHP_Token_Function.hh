<?hh

// @TODO: Work on getting this file to strict.

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenInterface;
use SebastianBergmann\TokenStream\TokenWithScopeAndVisibility;

class PHP_Token_Function extends TokenWithScopeAndVisibility {

  /**
   * @var array
   */
  protected Map<string, ?string> $arguments = Map {};
  protected bool $didArguments = false;

  /**
   * @var int
   */
  protected int $ccn = -1;

  /**
   * @var string
   */
  protected string $name = '';
  protected bool $didName = false;

  /**
   * @var string
   */
  protected string $signature = '';
  protected bool $didSignature = false;

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

      // --
      //
      // @TODO: Write a unit test around multi line abstract function definitions.
      //
      // JEO: I think this is the right thing for abstract multi line functions.
      // Needs a test case to prove.
      // --
      // if ($token instanceof PHP_Token_Semicolon) {
      //   $this->endOfDefinitionId = $token->getId();
      //   break;
      // }

    }

    $endOfDefinitionToken = $tokens->get($this->endOfDefinitionId);

    return $endOfDefinitionToken;

  }

  /**
   * @return array
   */
  public function getArguments(): Map<string, ?string> {

    if ($this->didArguments === true) {
      return $this->arguments;
    }

    $tokens = $this->tokenStream()->tokens();
    $typeDeclaration = null;

    // Search for first token inside brackets
    $i = $this->getId() + 2;

    while (!$tokens[$i - 1] instanceof PHP_Token_Open_Bracket) {
      $i++;
    }

    while (!$tokens[$i] instanceof PHP_Token_Close_Bracket) {

      $currentToken = $tokens->get($i);

      if ($currentToken instanceof PHP_Token_String) {
        $typeDeclaration = (string) $tokens[$i];
      } else if ($currentToken instanceof PHP_Token_Variable) {
        $this->arguments->set(strval($currentToken), $typeDeclaration);
        $typeDeclaration = null;
      }

      $i++;
    }

    $this->didArguments = true;
    return $this->arguments;
  }

  /**
   * @return string
   */
  public function getName(): string {

    if ($this->didName === true) {
      return $this->name;
    }

    $tokens = $this->tokenStream()->tokens();

    for ($i = $this->getId() + 1; $i < $tokens->count(); $i++) {

      $token = $tokens->get($i);
      $nextToken = $tokens->get($i + 1);

      if ($token instanceof PHP_Token_String) {
        $this->name = (string) $token;
        break;
      } else if ($token instanceof PHP_Token_Ampersand &&
                 $nextToken instanceof PHP_Token_String) {
        $this->name = (string) $nextToken;
        break;
      } else if ($token instanceof PHP_Token_Open_Bracket) {
        $this->name = 'anonymous function';
        break;
      }

    }

    // Handling php6/7 anon functions bs.
    // JEO: disabling for now.
    // if ($this->name != 'anonymous function') {
    //   for ($i = $this->id; $i; --$i) {
    //
    //     $token = $tokens->get($i);
    //
    //     if ($token instanceof PHP_Token_Namespace) {
    //       $this->name = $token->getName().'\\'.$this->name;
    //       break;
    //     }
    //
    //     if ($token instanceof PHP_Token_Interface) {
    //       break;
    //     }
    //
    //   }
    //
    // }

    $this->didName = true;
    return $this->name;
  }

  /**
   * @return int
   */
  public function getCCN(): int {

    if ($this->ccn !== -1) {
      return $this->ccn;
    }

    $this->ccn = 1;
    $start = $this->getId();
    $end = $this->getEndTokenId();
    $tokens = $this->tokenStream()->tokens();

    $startLine = $tokens[$start]->getLine();
    $endLine = $tokens[$end]->getLine();

    // error_log(
    //   "getCCN file=".
    //   $this->tokenStream()->getFilename().
    //   " function=".
    //   $this->getName().
    //   " start=$start startLine=$startLine end=$end endLine=$endLine",
    // );

    for ($i = $start; $i <= $end; $i++) {
      $token = $tokens->get($i);

      //echo "token=".get_class($token)."[$i] ccn=".$this->ccn."\n";

      if ($token instanceof PHP_Token_If ||
          $token instanceof PHP_Token_Elseif ||
          $token instanceof PHP_Token_For ||
          $token instanceof PHP_Token_Foreach ||
          $token instanceof PHP_Token_While ||
          $token instanceof PHP_Token_Case ||
          $token instanceof PHP_Token_Catch ||
          $token instanceof PHP_Token_Boolean_And ||
          $token instanceof PHP_Token_Logical_And ||
          $token instanceof PHP_Token_Boolean_Or ||
          $token instanceof PHP_Token_Logical_Or ||
          $token instanceof PHP_Token_Question_Mark) {
        $this->ccn++;
        continue;
      }

    }

    return $this->ccn;
  }

  /**
   * @return string
   */
  public function getSignature() {

    if ($this->didSignature === true) {
      return $this->signature;
    }

    $i = 0;
    $id = $this->getId();
    if ($this->getName() == 'anonymous function') {
      $this->signature = 'anonymous function';
      $i = $id + 1;
    } else {
      $this->signature = '';
      $i = $id + 2;
    }

    $tokens = $this->tokenStream()->tokens();

    for ($r = $i; $r < $tokens->count(); $r++) {

      $token = $tokens->get($r);

      if ($token instanceof PHP_Token_Open_Curly ||
          $token instanceof PHP_Token_Semicolon) {
        break;
      }

      $this->signature .= $token;

    }

    $this->signature = trim($this->signature);
    $this->didSignature = true;

    return $this->signature;
  }
}
