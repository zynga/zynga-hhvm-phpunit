<?hh // strict

namespace SebastianBergmann\TokenStream;

use SebastianBergmann\TokenStream\Token\Stream;
use SebastianBergmann\TokenStream\Token\Stream\CachingFactory;
use SebastianBergmann\TokenStream\TokenInterface;
use Zynga\CodeBase\V1\File;
use \Exception;

/**
 * A PHP token.
 */
abstract class Token implements TokenInterface {

  private string $_text = '';
  private int $_line = -1;
  private ?File $_file = null;
  private int $_id = -1;

  // --
  // Do not add a __constructor here. We've proven that the constructor overhead
  // will double instanciation time.
  //
  // Take a look at the simple test within:
  //   tests/performance/constructor
  //
  // --

  public function setAllAttributes(
    string $text,
    int $line,
    File $file,
    int $id,
  ): void {
    $this->_text = $text;
    $this->_line = $line;
    $this->_file = $file;
    $this->_id = $id;
  }

  public function getFile(): ?File {
    return $this->_file;
  }

  public function setFile(File $file): bool {
    $this->_file = $file;
    return true;
  }

  public function getText(): string {
    return $this->_text;
  }

  public function setText(string $text): bool {
    $this->_text = $text;
    return true;
  }

  public function getLine(): int {
    return $this->_line;
  }

  public function setLine(int $line): bool {
    $this->_line = $line;
    return true;
  }

  public function getId(): int {
    return $this->_id;
  }

  public function setId(int $id): bool {
    $this->_id = $id;
    return true;
  }

  /**
   * @return string
   */
  public function __toString(): string {
    return strval($this->_text);
  }

  public function tokenStream(): Stream {
    $file = $this->getFile();
    if ($file instanceof File) {
      return $file->stream();
    }
    throw new Exception(
      'tokenStream called on a token with no file use setFile()',
    );
  }

  public function getShortTokenName(): string {
    return str_replace(
      'SebastianBergmann\TokenStream\Tokens\PHP_Token_',
      '',
      static::class,
    );
  }
}
