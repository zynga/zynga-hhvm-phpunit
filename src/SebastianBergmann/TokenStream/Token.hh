<?hh // strict

namespace SebastianBergmann\TokenStream;

use SebastianBergmann\TokenStream\Token\Stream;
use SebastianBergmann\TokenStream\Token\Stream\CachingFactory;
use SebastianBergmann\TokenStream\TokenInterface;
use Zynga\CodeBase\V1\File;

/**
 * A PHP token.
 */
abstract class Token implements TokenInterface {

  protected string $text;
  protected int $line;
  private File $_parent;
  protected int $id;

  /**
   * @param string           $text
   * @param int              $line
   * @param Stream $tokenStream
   * @param int              $id
   */
  public function __construct(string $text, int $line, File $parent, int $id) {
    $this->text = $text;
    $this->line = $line;
    $this->_parent = $parent;
    $this->id = $id;
  }

  public function parent(): File {
    return $this->_parent;
  }

  public function getText(): string {
    return $this->text;
  }

  public function setText(string $text): bool {
    $this->text = $text;
    return true;
  }

  public function getLine(): int {
    return $this->line;
  }

  public function setLine(int $line): bool {
    $this->line = $line;
    return true;
  }

  public function getId(): int {
    return $this->id;
  }

  public function setId(int $id): bool {
    $this->id = $id;
    return true;
  }

  /**
   * @return string
   */
  public function __toString(): string {
    return $this->text;
  }

  public function tokenStream(): Stream {
    return $this->_parent->stream();
  }

  public function getShortTokenName(): string {
    return str_replace(
      'SebastianBergmann\TokenStream\Tokens\PHP_Token_',
      '',
      static::class,
    );
  }
}
