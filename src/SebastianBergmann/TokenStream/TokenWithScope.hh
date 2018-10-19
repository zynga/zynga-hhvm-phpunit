<?hh // strict

namespace SebastianBergmann\TokenStream;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\TokenInterface;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Class;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Close_Curly;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Curly_Open;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Comment;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Doc_Comment;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Function;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Namespace;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Open_Curly;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Public;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Private;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Static;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Semicolon;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Trait;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Whitespace;
use Zynga\CodeBase\V1\File;

abstract class TokenWithScope extends Token {
  /**
   * @var int
   */
  protected int $endTokenId = -1;

  private string $_docBlock = '';
  private bool $_didDocBlock = false;

  /**
   * Get the docblock for this token
   *
   * This method will fetch the docblock belonging to the current token. The
   * docblock must be placed on the line directly above the token to be
   * recognized.
   *
   * @return string|null Returns the docblock as a string if found
   */
  public function getDocblock(): string {

    if ($this->_didDocBlock === true) {
      return $this->_docBlock;
    }

    $id = $this->getId();
    $scanLimit = max($id - 100, 0);

    $tokens = $this->tokenStream()->tokens();

    $fileName = $this->tokenStream()->getFilename();

    for ($i = ($id - 2); $i > $scanLimit; $i--) {

      $token = $tokens->get($i);

      if (!$token instanceof TokenInterface) {
        break;
      }

      // rewind until we find a comment or something else.
      if ($token instanceof PHP_Token_Whitespace) {
        continue;
      }

      // modifiers don't count.
      if ($token instanceof PHP_Token_Public ||
          $token instanceof PHP_Token_Private ||
          $token instanceof PHP_Token_Static) {
        continue;
      }

      if ($token instanceof PHP_Token_Doc_Comment) {
        $this->_docBlock = $token->getText();
        break;
      } else if ($token instanceof PHP_Token_Comment) {
        $this->_docBlock = $token->getText();
        break;
      } else {
        break;
      }

    }

    $this->_didDocBlock = true;
    return $this->_docBlock;

  }

  /**
   * @return int
   */
  public function getEndTokenId(): int {

    if ($this->endTokenId !== -1) {
      return $this->endTokenId;
    }

    $block = 0;
    $tokens = $this->tokenStream()->tokens();

    for ( $i = $this->getId(); $i < $tokens->count(); $i++ ) {

      $token = $tokens->get($i);

      if ($token === null) {
        break;
      }

      if ($token instanceof PHP_Token_Open_Curly ||
          $token instanceof PHP_Token_Curly_Open) {

        $block++;

      } else if ($token instanceof PHP_Token_Close_Curly) {

        $block--;

        if ($block === 0) {
          $this->endTokenId = $token->getId();
          break;
        }

      } else if (($this instanceof PHP_Token_Function ||
                  $this instanceof PHP_Token_Namespace) &&
                 $token instanceof PHP_Token_Semicolon) {

        // This is to support singular line function or namespace suppport, we should consider 
        // swapping them to default to semicolon parser.
        if ($block === 0) {
          $this->endTokenId = $token->getId();
          break;
        }

      }

    }

    if ($this->endTokenId === -1) {
      $this->endTokenId = $this->getId();
    }

    return $this->endTokenId;
  }

  public function getEndOfDefinitionLineNo(): int {
    return $this->getEndLine();
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
