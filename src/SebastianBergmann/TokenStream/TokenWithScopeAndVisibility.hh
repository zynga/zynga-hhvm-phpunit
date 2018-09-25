<?hh // strict

namespace SebastianBergmann\TokenStream;

use SebastianBergmann\TokenStream\TokenWithScope;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Abstract;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Final;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Private;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Protected;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Public;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Static;

abstract class TokenWithScopeAndVisibility extends TokenWithScope {
  const string PUBLIC_VISIBILITY = 'public';
  const string PRIVATE_VISIBILITY = 'private';
  const string PROTECTED_VISIBILITY = 'protected';
  const string STATIC_KEYWORD = 'static';
  const string FINAL_KEYWORD = 'final';
  const string ABSTRACT_KEYWORD = 'abstract';

  private bool $_didVisibility = false;
  private string $_tokenVisibility = '';
  private bool $_didKeywords = false;
  private array<string> $_keywords = array();

  private function setVisibility(string $visibility): string {
    $this->_didVisibility = true;
    $this->_tokenVisibility = $visibility;
    return $this->_tokenVisibility;
  }

  /**
   * @return string
   */
  public function getVisibility(): string {

    if ($this->_didVisibility === true) {
      return $this->_tokenVisibility;
    }

    $tokens = $this->tokenStream()->tokens();

    $id = $this->getId();

    /*
    error_log(
      $this->tokenStream()->getFilename().
      ' functionId='.
      $id.
      ' name='.
      $this->getText(),
    );
    */

    for ($i = $id; $i > $id - 10; $i--) {

      $token = $tokens->get($i);

      //error_log($i.'='.get_class($token));

      if ($token instanceof PHP_Token_Public) {
        return $this->setVisibility(self::PUBLIC_VISIBILITY);
      } else if ($token instanceof PHP_Token_Private) {
        return $this->setVisibility(self::PRIVATE_VISIBILITY);
      } else if ($token instanceof PHP_Token_Protected) {
        return $this->setVisibility(self::PROTECTED_VISIBILITY);
      }

    }

    return $this->setVisibility('');

  }

  /**
   * @return string
   */
  public function getKeywords(): string {

    if ( $this->_didKeywords === true ) {
      return implode(',', $this->_keywords);
    }

    $tokens = $this->tokenStream()->tokens();

    $id = $this->getId();

    for ($i = $id; $i > $id - 10; $i--) {

      $token = $tokens->get($i);

      if ($token instanceof PHP_Token_Private ||
          $token instanceof PHP_Token_Protected ||
          $token instanceof PHP_Token_Public) {
        continue;
      }

      if ( $token instanceof PHP_Token_Static) {
        $this->_keywords[] = self::STATIC_KEYWORD;
        continue;
      }
      if ( $token instanceof PHP_Token_Final ) {
        $this->_keywords[] = self::FINAL_KEYWORD;
        continue;
      }
      if ( $token instanceof PHP_Token_Abstract ) {
        $this->_keywords[] = self::ABSTRACT_KEYWORD;
        continue;
      }
    }

    $this->_didKeywords = true;

    return implode(',', $this->_keywords);

  }
}
