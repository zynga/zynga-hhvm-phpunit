<?hh // strict

namespace SebastianBergmann\CodeCoverage\Driver\HHVM;

// --
// TODO: let's make this utilize object references and not string comparisons.
// --
class LineStack {

  private static Vector<string>
    $executableTokens = Vector {
      'PHP_Token_RETURN',
      'PHP_Token_THROW',
      'PHP_Token_EQUAL',
      'PHP_Token_IF',
      'PHP_Token_UNSET',
      'PHP_Token_OBJECT_OPERATOR',
      'PHP_Token_THROW',
    };

  private static Vector<string>
    $ifTokens = Vector {'PHP_Token_IF', 'PHP_Token_ELSEIF'};

  private static Vector<string>
    $fileInclusion = Vector {
      'PHP_Token_REQUIRE',
      'PHP_Token_REQUIRE_ONCE',
      'PHP_Token_INCLUDE',
      'PHP_Token_INCLUDE_ONCE',
    };

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
    if ($offset > -1) {
      return true;
    }
    return false;
  }

  public function containsAny(Vector<string> $tokens): bool {
    foreach ($tokens as $token) {
      if ($this->contains($token) === true) {
        return true;
      }
    }
    return false;
  }

  public function containsAll(Vector<string> $tokens): bool {

    $matchCount = 0;

    foreach ($tokens as $token) {
      if ($this->contains($token)) {
        $matchCount++;
      } else {
        // if there's a required token, and we are missing it.. early out.
        //return false;
      }
    }

    if ($tokens->count() == $matchCount) {
      return true;
    }

    return false;

  }

  public function isAbstractFunction(): bool {
    if ($this->contains('PHP_Token_ABSTRACT') &&
        $this->contains('PHP_Token_FUNCTION')) {
      return true;
    }
    return false;
  }

  public function doesContainReturn(): bool {
    if ($this->contains('PHP_Token_RETURN') === true) {
      return true;
    }
    return false;
  }

  public function doesContainSemiColon(): bool {
    if ($this->contains('PHP_Token_SEMICOLON') === true) {
      return true;
    }
    return false;
  }

  public function doesContainStartingCurly(): bool {
    if ($this->contains('PHP_Token_OPEN_CURLY') === true) {
      return true;
    }
    return false;
  }

  public function doesContainEndingCurly(): bool {
    if ($this->contains('PHP_Token_CLOSE_CURLY') === true) {
      return true;
    }
    return false;
  }

  public function doesContainIf(): bool {
    return $this->containsAny(self::$ifTokens);
  }

  public function isFileInclusion(): bool {
    return $this->containsAny(self::$fileInclusion);
  }

  public function isExecutable(): bool {

    // We don't count require_once / include_once as executable code.
    if ($this->isFileInclusion() === true) {
      return false;
    }

    // --
    // If there is a parseable / executable token this counts as a line that 'could' be executed.
    // --
    return $this->containsAny(self::$executableTokens);

  }

}
