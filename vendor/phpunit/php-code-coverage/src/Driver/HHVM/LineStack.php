<?hh // strict

namespace SebastianBergmann\CodeCoverage\Driver\HHVM;

class LineStack {
  private Vector<string> $_stack;
  private string $_text;

  public function __construct() {
    $this->_stack = Vector {};
    $this->_text = '';
  }

  public function toJSON(): string {
    return json_encode($this->_stack->toArray());
  }

  public function clear(): bool {
    $this->_stack->clear();
    $this->_text = '';
    return true;
  }

  public function getLineText(): string {
    return $this->_text;
  }

  public function addLineText(string $text): bool {
    $this->_text .= $text;
    return true;
  }

  public function addToken(string $token): bool {
    $this->_stack[] = $token;
    return true;
  }

  public function contains(string $token): bool {
    // Our version of hack doesn't support contains at this time.
    // return $this->_stack->contains($token);
    $offset = $this->_stack->linearSearch($token);
    if ( $offset > -1 ) {
      return true;
    }
    return false;
  }

  public function containsAny(Vector<string> $tokens): bool {
    foreach ( $tokens as $token ) {
      if ( $this->contains($token) === true ) {
        return true;
      }
    }
    return false;
  }

  public function containsAll(Vector<string> $tokens): bool {

    $matchCount = 0;

    foreach ( $tokens as $token ) {
      if ( $this->contains($token) ) {
        $matchCount++;
      }
    }

    if ( $tokens->count() == $matchCount ) {
      return true;
    }

    return false;

  }

  public function isAbstractFunction(): bool {
    if ( $this->contains('PHP_Token_ABSTRACT') && $this->contains('PHP_Token_FUNCTION') ) {
      return true;
    }
    return false;
  }

  public function isUseStatement(): bool {
    if ( $this->contains('PHP_Token_USE') ) {
      return true;
    }
    return false;
  }

  public function isThrowStatement(): bool {
    if ($this->contains('PHP_Token_THROW') ) {
      return true;
    }
    return false;
  }

  public function hasFunction(): bool {
    if ($this->contains('PHP_Token_FUNCTION') ) {
      return true;
    }
    return false;
  }

  public function doesContainReturn(): bool {
    if ( $this->contains('PHP_Token_RETURN') === true ) {
      return true;
    }
    return false;
  }

  public function doesContainSemiColon(): bool {
    if ( $this->contains('PHP_Token_SEMICOLON') === true ) {
      return true;
    }
    return false;
  }

  public function doesContainStartingCurly(): bool {
    if ( $this->contains('PHP_Token_OPEN_CURLY') === true ) {
      return true;
    }
    return false;
  }

  public function doesContainEndingCurly(): bool {
    if ( $this->contains('PHP_Token_CLOSE_CURLY') === true ) {
      return true;
    }
    return false;
  }

  public function doesContainIf(): bool {
    $ifs = Vector {};
    $ifs[] = 'PHP_Token_IF';
    $ifs[] = 'PHP_Token_ELSEIF';
    return $this->containsAny($ifs);
  }

  public function isFileInclusion(): bool {
    $fileInclusion = Vector {};
    $fileInclusion[] = 'PHP_Token_REQUIRE';
    $fileInclusion[] = 'PHP_Token_REQUIRE_ONCE';
    $fileInclusion[] = 'PHP_Token_INCLUDE';
    $fileInclusion[] = 'PHP_Token_INCLUDE_ONCE';
    $fileInclusion[] = 'PHP_Token_USE';
    return $this->containsAny($fileInclusion);
  }

  public function isExecutable(): bool {

    // --
    // If there is a parseable / executable token this counts as a line that 'could' be executed.
    // --
    $executableTokens = Vector {};
    $executableTokens[] = 'PHP_Token_RETURN';
    $executableTokens[] = 'PHP_Token_THROW';
    $executableTokens[] = 'PHP_Token_EQUAL';
    $executableTokens[] = 'PHP_Token_IF';
    $executableTokens[] = 'PHP_Token_OBJECT_OPERATOR';
    //$executableTokens[] = 'PHP_Token_USE';

    return $this->containsAny($executableTokens);

  }

}
