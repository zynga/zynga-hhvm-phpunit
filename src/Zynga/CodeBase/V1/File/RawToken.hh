<?hh // strict

namespace Zynga\CodeBase\V1\File;

class RawToken {
  private int $_tokenId = -1;
  private string $_text = '';
  private int $_lineNo = -1;

  public function setAll(int $tokenId, string $text, int $lineNo): void {
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
