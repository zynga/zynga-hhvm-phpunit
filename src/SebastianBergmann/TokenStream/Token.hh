<?hh // strict

namespace SebastianBergmann\TokenStream;

use SebastianBergmann\TokenStream\Token\Stream;
use SebastianBergmann\TokenStream\Token\Stream\CachingFactory;
use SebastianBergmann\TokenStream\TokenInterface;
use Zynga\CodeBase\V1\File;
use SebastianBergmann\TokenStream\Token\Types;

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

  final public function setAllAttributes(
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

  final public function getFile(): ?File {
    return $this->_file;
  }

  final public function setFile(File $file): bool {
    $this->_file = $file;
    return true;
  }

  final public function getText(): string {
    return $this->_text;
  }

  final public function setText(string $text): bool {
    $this->_text = $text;
    return true;
  }

  public function getEndTokenId(): int {
    return $this->_id;
  }

  public function getEndLine(): int {
    return $this->getLine();
  }

  final public function getLine(): int {
    return $this->_line;
  }

  final public function setLine(int $line): bool {
    $this->_line = $line;
    return true;
  }

  final public function getId(): int {
    return $this->_id;
  }

  final public function setId(int $id): bool {
    $this->_id = $id;
    return true;
  }

  final public function __toString(): string {
    return strval($this->_text);
  }

  final public function tokenStream(): Stream {
    $file = $this->getFile();
    if ($file instanceof File) {
      return $file->stream();
    }
    throw new Exception(
      'tokenStream called on a token with no file use setFile()',
    );
  }

  // --
  // JEO: Why not have a str_replace here to change the namespace name down
  // to this short name? We run this function a few hundred thousand times ;)
  // --
  abstract public function getShortTokenName(): string;

  // --
  // Most tokens don't have continuation concepts, however some block components
  //  do, such as:
  //  - If, Else, ElseIf
  //  - Try, Catch, Finally
  // --
  public function hasContinuation(): bool {
    $id = $this->getContinuationTokenId();

    if ($id > -1) {
      return true;
    }

    return false;
  }

  public function getContinuationTokenId(): int {
    return -1;
  }

  public function getContinuationToken(): ?TokenInterface {

    $id = $this->getContinuationTokenId();

    // there isn't a continuation token for this token.
    if ($id == -1) {
      return null;
    }

    $tokens = $this->tokenStream()->tokens();

    return $tokens->get($id);

  }

  public function getEndOfDefinitionLineNo(): int {

    $token = $this->getEndOfDefinitionToken();

    if ($token instanceof TokenInterface) {
      return $token->getLine();
    }

    return $this->getLine();

  }

  // --
  // JEO: This is by default the same value as the lineno, some of the tokens have overloaded
  // this defintition to allow you to find the end of a code block or definition.
  // --
  public function getEndOfDefinitionTokenId(): int {
    return -1;
  }

  public function getEndOfDefinitionToken(): ?TokenInterface {

    $id = $this->getEndOfDefinitionTokenId();

    if ($id == -1) {
      return null;
    }

    $tokens = $this->tokenStream()->tokens();

    $token = $tokens->get($id);

    return $token;

  }

}
