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
  /**
   * @return string
   */
  public function getVisibility(): string {

    $tokens = $this->tokenStream->tokens();

    for ($i = $this->id - 2; $i > $this->id - 7; $i -= 2) {

      $token = $tokens->get($i);

      if ($token instanceof PHP_Token_Private ||
          $token instanceof PHP_Token_Protected ||
          $token instanceof PHP_Token_Public) {
        $visbility = strtolower(
          str_replace(
            'SebastianBergmann\TokenStream\Tokens\PHP_Token_',
            '',
            get_class($token),
          ),
        );
        return $visbility;
      }
    }
    return '';
  }

  /**
   * @return string
   */
  public function getKeywords(): string {

    $keywords = array();

    $tokens = $this->tokenStream->tokens();

    for ($i = $this->id - 2; $i > $this->id - 7; $i -= 2) {

      $token = $tokens->get($i);

      if ($token instanceof PHP_Token_Private ||
          $token instanceof PHP_Token_Protected ||
          $token instanceof PHP_Token_Public) {
        continue;
      }

      if ($token instanceof PHP_Token_Static ||
          $token instanceof PHP_Token_Final ||
          $token instanceof PHP_Token_Abstract) {
        $keywords[] = strtolower(
          str_replace(
            'SebastianBergmann\TokenStream\Tokens\PHP_Token_',
            '',
            get_class($token),
          ),
        );
      }
    }

    return implode(',', $keywords);
  }
}
