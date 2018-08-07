<?hh // strict

namespace SebastianBergmann\CodeCoverage\Driver\HHVM;

use SebastianBergmann\TokenStream\TokenInterface;

use SebastianBergmann\TokenStream\Tokens\PHP_Token_Abstract;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Close_Curly;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Elseif;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Equal;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Function;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_If;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Include;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Include_Once;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Object_Operator;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Open_Curly;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Return;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Require;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Require_Once;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Semicolon;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Throw;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Unset;

class LineStack {

  private static Vector<classname<TokenInterface>>
    $executableTokens = Vector {
      PHP_Token_Return::class,
      PHP_Token_Throw::class,
      PHP_Token_Equal::class,
      PHP_Token_If::class,
      PHP_Token_Unset::class,
      PHP_Token_Object_Operator::class,
    };

  private static Vector<classname<TokenInterface>>
    $ifTokens = Vector {PHP_Token_If::class, PHP_Token_Elseif::class};

  private static Vector<classname<TokenInterface>>
    $fileInclusion = Vector {
      PHP_Token_Require::class,
      PHP_Token_Require_Once::class,
      PHP_Token_Include::class,
      PHP_Token_Include_Once::class,
    };

  private Vector<TokenInterface> $_stack;
  private string $_text;

  public function __construct() {
    $this->_stack = Vector {};
    $this->_text = '';
  }

  public function toJSON(): string {
    return json_encode($this->_stack->toArray());
  }

  public function clear(): bool {
    $this->_stack->clear();
    $this->_text = '';
    return true;
  }

  public function getLineText(): string {
    return $this->_text;
  }

  public function addLineText(string $text): bool {
    $this->_text .= $text;
    return true;
  }

  public function addToken(TokenInterface $token): bool {
    $this->_stack->add($token);
    return true;
  }

  public function contains(classname<TokenInterface> $token): bool {
    foreach ($this->_stack as $stackToken) {
      if ($stackToken instanceof $token) {
        return true;
      }
    }
    return false;
  }

  public function containsAny(
    Vector<classname<TokenInterface>> $tokens,
  ): bool {
    foreach ($tokens as $token) {
      if ($this->contains($token) === true) {
        return true;
      }
    }
    return false;
  }

  public function containsAll(
    Vector<classname<TokenInterface>> $tokens,
  ): bool {

    // Required token count exceeds the possible for this stack item.
    if ($tokens->count() > $this->_stack->count()) {
      return false;
    }

    $matchCount = 0;

    foreach ($tokens as $token) {
      if ($this->contains($token)) {
        $matchCount++;
      } else {
        // if there's a required token, and we are missing it.. early out.
        //return false;
      }
    }

    if ($tokens->count() == $matchCount) {
      return true;
    }

    return false;

  }

  public function isAbstractFunction(): bool {
    if ($this->containsAll(
          Vector {PHP_Token_Abstract::class, PHP_Token_Function::class},
        )) {
      return true;
    }
    return false;
  }

  public function doesContainReturn(): bool {
    if ($this->contains(PHP_Token_Return::class) === true) {
      return true;
    }
    return false;
  }

  public function doesContainSemiColon(): bool {
    if ($this->contains(PHP_Token_Semicolon::class) === true) {
      return true;
    }
    return false;
  }

  public function doesContainStartingCurly(): bool {
    if ($this->contains(PHP_Token_Open_Curly::class) === true) {
      return true;
    }
    return false;
  }

  public function doesContainEndingCurly(): bool {
    if ($this->contains(PHP_Token_Close_Curly::class) === true) {
      return true;
    }
    return false;
  }

  public function doesContainIf(): bool {
    return $this->containsAny(self::$ifTokens);
  }

  public function isFileInclusion(): bool {
    return $this->containsAny(self::$fileInclusion);
  }

  public function isExecutable(): bool {

    // We don't count require_once / include_once as executable code.
    if ($this->isFileInclusion() === true) {
      return false;
    }

    // --
    // If there is a parseable / executable token this counts as a line that 'could' be executed.
    // --
    return $this->containsAny(self::$executableTokens);

  }

}
