<?hh // strict

namespace SebastianBergmann\TokenStream;

use SebastianBergmann\TokenStream\Token\Stream as PHP_Token_Stream;
use SebastianBergmann\TokenStream\TokenInterface;

/**
 * A PHP token.
 */
abstract class Token implements TokenInterface {
  /**
   * @var string
   */
  protected string $text;

  /**
   * @var int
   */
  protected int $line;

  /**
   * @var PHP_Token_Stream
   */
  protected PHP_Token_Stream $tokenStream;

  /**
   * @var int
   */
  protected int $id;

  /**
   * @param string           $text
   * @param int              $line
   * @param PHP_Token_Stream $tokenStream
   * @param int              $id
   */
  public function __construct(
    string $text,
    int $line,
    PHP_Token_Stream $tokenStream,
    int $id,
  ) {
    $this->text = $text;
    $this->line = $line;
    $this->tokenStream = $tokenStream;
    $this->id = $id;
  }

  /**
   * @return string
   */
  public function __toString(): string {
    return $this->text;
  }

  /**
   * @return int
   */
  public function getLine(): int {
    return $this->line;
  }
}
