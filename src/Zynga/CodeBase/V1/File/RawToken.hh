<?hh // strict

namespace Zynga\CodeBase\V1\File;

class RawToken {
  private int $_tokenId;
  private string $_text;
  private int $_lineNo;

  public function __construct(int $tokenId, string $text, int $lineNo) {
    $this->_tokenId = $tokenId;
    $this->_text = $text;
    $this->_lineNo = $lineNo;
  }

  public function getTokenId(): int {
    return $this->_tokenId;
  }

  public function getText(): string {
    return $this->_text;
  }

  public function getLineNo(): int {
    return $this->_lineNo;
  }

}
