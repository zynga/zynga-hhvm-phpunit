<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenWithScope;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Ns_Seperator;
use SebastianBergmann\TokenStream\Token\Types;
use SebastianBergmann\TokenStream\TokenInterface;

class PHP_Token_Namespace extends TokenWithScope {

  private bool $_didNsName = false;
  private string $_nsName = '';

  /**
   * @return string
   */
  public function getName(): string {

    if ($this->_didNsName == true) {
      return $this->_nsName;
    }

    $id = $this->getId();

    $tokens = $this->tokenStream()->tokens();

    // give this scan a very large leash.
    for ($i = $id; $i < ($id + 100); $i++) {

      $token = $tokens->get($i);

      if (!$token instanceof TokenInterface) {
        continue;
      }

      if ($token instanceof PHP_Token_Whitespace) {
        continue;
      }

      if ($token instanceof PHP_Token_Open_Curly) {
        break;
      }

      if ($token instanceof PHP_Token_Semicolon) {
        break;
      }

      $this->_nsName .= $token->getText();

    }

    $this->_didNsName = true;
    return $this->_nsName;

  }

  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }

  public function getShortTokenName(): string {
    return 'Namespace';
  }

}
