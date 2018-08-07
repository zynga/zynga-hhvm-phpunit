<?hh // strict

namespace SebastianBergmann\TokenStream;

use SebastianBergmann\TokenStream\Token\Stream;
use SebastianBergmann\TokenStream\Token\Stream\CachingFactory;
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

  private int $tokenStreamId;

  /**
   * @var int
   */
  protected int $id;

  /**
   * @param string           $text
   * @param int              $line
   * @param Stream $tokenStream
   * @param int              $id
   */
  public function __construct(
    string $text,
    int $line,
    int $tokenStreamId,
    int $id,
  ) {
    $this->text = $text;
    $this->line = $line;
    $this->tokenStreamId = $tokenStreamId;
    $this->id = $id;
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

  public function getTokenStreamId(): int {
    return $this->tokenStreamId;
  }

  public function setTokenStreamId(int $tokenStreamId): bool {
    $this->tokenStreamId = $tokenStreamId;
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
    return CachingFactory::getByStreamId($this->tokenStreamId);
  }

}
