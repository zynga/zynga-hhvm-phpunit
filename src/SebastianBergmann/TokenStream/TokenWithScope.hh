<?hh // strict

namespace SebastianBergmann\TokenStream;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\TokenInterface;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Class;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Close_Curly;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Curly_Open;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Doc_Comment;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Function;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Namespace;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Open_Curly;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Semicolon;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Trait;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Whitespace;

abstract class TokenWithScope extends Token {
  /**
   * @var int
   */
  protected int $endTokenId = -1;

  /**
   * Get the docblock for this token
   *
   * This method will fetch the docblock belonging to the current token. The
   * docblock must be placed on the line directly above the token to be
   * recognized.
   *
   * @return string|null Returns the docblock as a string if found
   */
  public function getDocblock(): ?string {
    $tokens = $this->tokenStream()->tokens();
    $currentLineNumber = $tokens[$this->id]->getLine();
    $prevLineNumber = $currentLineNumber - 1;

    for ($i = $this->id - 1; $i; $i--) {

      if (!$tokens->containsKey($i)) {
        return null;
      }

      $token = $tokens->get($i);

      if (!$token instanceof TokenInterface) {
        return null;
      }

      if ($token instanceof PHP_Token_Function ||
          $token instanceof PHP_Token_Class ||
          $token instanceof PHP_Token_Trait) {
        // Some other trait, class or function, no docblock can be
        // used for the current token
        break;
      }

      $line = $token->getLine();

      if ($line == $currentLineNumber ||
          ($line == $prevLineNumber &&
           $token instanceof PHP_Token_Whitespace)) {
        continue;
      }

      if ($line < $currentLineNumber &&
          !$token instanceof PHP_Token_Doc_Comment) {
        break;
      }

      return (string) $tokens[$i];
    }

    return null;

  }

  /**
   * @return int
   */
  public function getEndTokenId(): int {

    if ($this->endTokenId !== -1) {
      return $this->endTokenId;
    }

    $block = 0;
    $i = $this->id;
    $tokens = $this->tokenStream()->tokens();

    while ($this->endTokenId === -1 && $tokens->containsKey($i)) {

      $token = $tokens->get($i);

      if ($token instanceof PHP_Token_Open_Curly ||
          $token instanceof PHP_Token_Curly_Open) {
        $block++;
      } else if ($token instanceof PHP_Token_Close_Curly) {
        $block--;

        if ($block === 0) {
          $this->endTokenId = $i;
        }
      } else if (($this instanceof PHP_Token_Function ||
                  $this instanceof PHP_Token_Namespace) &&
                 $token instanceof PHP_Token_Semicolon) {
        if ($block === 0) {
          $this->endTokenId = $i;
        }
      }

      $i++;
    }

    if ($this->endTokenId === -1) {
      $this->endTokenId = $this->id;
    }

    return $this->endTokenId;
  }

  /**
   * @return int
   */
  public function getEndLine(): int {
    $endLine = 0;

    $endToken = $this->tokenStream()->get($this->getEndTokenId());

    if ($endToken instanceof TokenInterface) {
      $endLine = $endToken->getLine();
    }

    return $endLine;

  }
}
