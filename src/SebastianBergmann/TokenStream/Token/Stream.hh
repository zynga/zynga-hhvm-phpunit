<?hh // strict

namespace SebastianBergmann\TokenStream\Token;

/*
 * This file is part of php-token-stream.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use SebastianBergmann\TokenStream\TokenInterface;
use Zynga\CodeBase\V1\File;

use \Exception;
use \OutOfBoundsException;

/**
 * A stream of PHP tokens.
 */
class Stream {

  /**
   * @var array
   */
  private Vector<TokenInterface> $tokens = Vector {};

  // Map of lineNumber to tokens on that line.
  private Map<int, Vector<TokenInterface>> $_lineToTokens = Map {};

  /**
   * @var int
   */
  private int $position = 0;

  /**
   * @var array
   */
  protected Map<string, int>
    $linesOfCode = Map {'loc' => 0, 'cloc' => 0, 'ncloc' => 0};

  /**
   * @var array
   */
  protected Map<int, string> $lineToFunctionMap = Map {};

  private File $_parent;
  /**
   * Constructor.
   *
   * @param string $fileName
   */
  public function __construct(File $parent) {

    $this->_parent = $parent;

  }

  public function parent(): File {
    return $this->_parent;
  }

  public function get(int $offset): ?TokenInterface {
    return $this->tokens->get($offset);
  }

  /**
   * @return string
   */
  public function __toString(): string {
    $buffer = '';

    foreach ($this->tokens as $token) {
      $buffer .= $token;
    }

    return $buffer;
  }

  /**
   * @return string
   */
  public function getFilename(): string {
    return $this->_parent->getFile();
  }

  public function getLineCount(): int {
    $locEntry = $this->linesOfCode->get('loc');
    if (is_int($locEntry)) {
      return $locEntry;
    }
    return -1;
  }

  /**
   * @return int
   */
  public function count(): int {
    return $this->tokens->count();
  }

  /**
   * @return Vector<TokenInterface>[]
   */
  public function tokens(): Vector<TokenInterface> {
    return $this->tokens;
  }

  /**
   * Returns the name of the function or method a line belongs to.
   *
   * @return string or null if the line is not in a function or method
   */
  public function getFunctionForLine(int $line): ?string {
    return $this->lineToFunctionMap->get($line);
  }

  /**
   * @return array
   */
  public function getLinesOfCode(): Map<string, int> {
    return $this->linesOfCode;
  }

  /**
   */
  public function rewind(): void {
    $this->position = 0;
  }

  /**
   * @return bool
   */
  public function valid(): bool {
    return $this->tokens->containsKey($this->position);
  }

  /**
   * @return int
   */
  public function key(): int {
    return $this->position;
  }

  /**
   * @return PHP_Token
   */
  public function current(): ?TokenInterface {
    return $this->tokens->get($this->position);
  }

  /**
   */
  public function next(): void {
    $this->position++;
  }

  /**
   * @param int $offset
   *
   * @return bool
   */
  public function offsetExists(int $offset): bool {
    return $this->tokens->containsKey($offset);
  }

  /**
   * @param int $offset
   *
   * @return mixed
   *
   * @throws OutOfBoundsException
   */
  public function offsetGet(int $offset): TokenInterface {
    $token = $this->tokens->get($offset);
    if (!$token instanceof TokenInterface) {
      throw new OutOfBoundsException(
        sprintf('No token at position "%s"', $offset),
      );
    }
    return $token;
  }

  public function addToken(TokenInterface $token): bool {
    $this->tokens->add($token);
    $this->addTokenToLine($token);
    return true;
  }

  public function addTokenToLine(TokenInterface $token): bool {

    $lineNo = $token->getLine();

    if (!$this->_lineToTokens->containsKey($lineNo)) {
      $this->_lineToTokens->set($lineNo, Vector {});
    }

    $this->_lineToTokens[$lineNo]->add($token);

    return true;

  }

  public function getLineToTokens(int $lineNo): Vector<TokenInterface> {
    $tokens = $this->_lineToTokens->get($lineNo);

    if ($tokens instanceof Vector) {
      return $tokens;
    }

    return Vector {};

  }

  public function getLineToTokensForLine(): Map<int, Vector<TokenInterface>> {
    return $this->_lineToTokens;
  }

  /**
   * @param int $offset
   *
   * @throws OutOfBoundsException
   */
  public function offsetUnset(int $offset): void {

    if (!$this->offsetExists($offset)) {
      throw new OutOfBoundsException(
        sprintf('No token at position "%s"', $offset),
      );
    }

    $this->tokens->removeKey($offset);

  }

  /**
   * Seek to an absolute position.
   *
   * @param int $position
   *
   * @throws OutOfBoundsException
   */
  public function seek(int $position): void {

    $this->position = $position;

    if (!$this->valid()) {
      throw new OutOfBoundsException(
        sprintf('No token at position "%s"', $this->position),
      );
    }
  }

  /**
   * @param string $name
   * @param int    $startLine
   * @param int    $endLine
   */
  private function addFunctionToMap(
    string $name,
    int $startLine,
    int $endLine,
  ): void {
    for ($line = $startLine; $line <= $endLine; $line++) {
      $this->lineToFunctionMap->set($line, $name);
    }
  }
}
