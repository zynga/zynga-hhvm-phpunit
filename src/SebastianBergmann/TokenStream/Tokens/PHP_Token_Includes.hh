<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token as PHP_Token;

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
    $token = $tokens->get($this->id);

    $tokenOffset = $this->tokenStream()->get($this->id + 2);

    if ($tokenOffset instanceof PHP_Token_Constant_Encapsed_String) {
      $this->name = trim(strval($tokenOffset), "'\"");
      $this->type = strtolower(
        str_replace(
          'SebastianBergmann\TokenStream\Tokens\PHP_Token_',
          '',
          get_class($token),
        ),
      );
    }

    $this->didProcess = true;

  }

}
