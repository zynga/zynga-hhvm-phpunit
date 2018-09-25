<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token as PHP_Token;
use SebastianBergmann\TokenStream\Token\Types;
use SebastianBergmann\TokenStream\TokenInterface;

abstract class PHP_Token_Includes extends PHP_Token {

  protected bool $didProcess = false;

  /**
   * @var string
   */
  protected string $name = '';

  /**
   * @var string
   */
  protected string $type = '';

  /**
   * @return string
   */
  public function getName(): string {

    if ($this->didProcess === false) {
      $this->process();
    }

    return $this->name;
  }

  /**
   * @return string
   */
  public function getType(): string {

    if ($this->didProcess === false) {
      $this->process();
    }

    return $this->type;
  }

  private function process(): void {

    $tokens = $this->tokenStream()->tokens();
    $id = $this->getId();
    $this->type = strtolower($this->getShortTokenName());

    for ($i = $id; $i < 100; $i++) {

      $token = $tokens->get($i);

      if (!$token instanceof TokenInterface) {
        continue;
      }

      // found the end of a inclusion statement.
      if ($token instanceof PHP_Token_Semicolon) {
        break;
      }

      if ($token instanceof PHP_Token_Whitespace) {
        continue;
      }

      $this->name .= trim($token->getText(), "'\"");

    }

    $this->didProcess = true;

  }

  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }

}
