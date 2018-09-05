<?hh // strict

namespace Zynga\CodeBase\V1;

use SebastianBergmann\CodeCoverage\Driver;
use SebastianBergmann\CodeCoverage\Driver\HHVM\ExecBlock;
use SebastianBergmann\CodeCoverage\Driver\HHVM\Logging as HHVM_Logging;
use SebastianBergmann\TokenStream\Token\Stream;
use SebastianBergmann\TokenStream\Token\Stream\Scanner as StreamScanner;
use SebastianBergmann\TokenStream\Token\Stream\Parser as StreamParser;
use SebastianBergmann\TokenStream\Token\Stream\CachingFactory;

use SebastianBergmann\TokenStream\Tokens\PHP_Token_Class;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Close_Curly;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Comment;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Function;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Return;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Throw;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Variable;

use SebastianBergmann\TokenStream\TokenInterface;

use Zynga\CodeBase\V1\File\Classes;
use Zynga\CodeBase\V1\File\Functions;
use Zynga\CodeBase\V1\File\Inclusions;
use Zynga\CodeBase\V1\File\Interfaces;
use Zynga\CodeBase\V1\File\LineExecutionState;
use Zynga\CodeBase\V1\File\RawTokens;
use Zynga\CodeBase\V1\File\Source;
use Zynga\CodeBase\V1\File\Stats;
use Zynga\CodeBase\V1\File\Traits;

class File {
  private bool $_didInit;
  private string $_file;
  private Map<int, Map<string, bool>> $_lineToTests;
  private Map<int, Vector<TokenInterface>> $_lineToTokens;
  private int $_startLine;
  private int $_endLine;

  private ?Classes $_classes;
  private ?Functions $_functions;
  private ?Inclusions $_inclusions;
  private ?Interfaces $_interfaces;
  private ?LineExecutionState $_lineExecutionState;
  private ?RawTokens $_rawTokens;
  private ?Source $_source;
  private ?Stats $_stats;
  private ?Stream $_stream;
  private ?Traits $_traits;

  public function __construct(string $file) {
    $this->_didInit = false;
    $this->_file = $file;
    $this->_lineToTests = Map {};
    $this->_lineToTokens = Map {};
    $this->_startLine = -1;
    $this->_endLine = -1;

    $this->_classes = null;
    $this->_functions = null;
    $this->_inclusions = null;
    $this->_interfaces = null;
    $this->_lineExecutionState = null;
    $this->_rawTokens = null;
    $this->_source = null;
    $this->_stats = null;
    $this->_stream = null;
    $this->_traits = null;

  }

  public function getFile(): string {
    return $this->_file;
  }

  public function source(): Source {
    if ($this->_source instanceof Source) {
      return $this->_source;
    }

    $this->_source = new Source($this);
    return $this->_source;

  }

  public function rawTokens(): RawTokens {
    if ($this->_rawTokens instanceof RawTokens) {
      return $this->_rawTokens;
    }

    $this->_rawTokens = new RawTokens($this);
    return $this->_rawTokens;

  }

  public function stats(): Stats {

    if ($this->_stats instanceof Stats) {
      return $this->_stats;
    }

    $this->_stats = new Stats($this);
    return $this->_stats;

  }

  public function lineExecutionState(): LineExecutionState {

    // We can only call this -after- stream has already populated.
    if ($this->_lineExecutionState instanceof LineExecutionState) {
      return $this->_lineExecutionState;
    }

    $this->_lineExecutionState = new LineExecutionState($this);

    return $this->_lineExecutionState;

  }

  public function functions(): Functions {

    if ($this->_functions instanceof Functions) {
      return $this->_functions;
    }

    $this->_functions = new Functions($this);

    return $this->_functions;

  }

  public function classes(): Classes {

    if ($this->_classes instanceof Classes) {
      return $this->_classes;
    }

    $this->_classes = new Classes($this);

    return $this->_classes;

  }

  public function traits(): Traits {

    if ($this->_traits instanceof Traits) {
      return $this->_traits;
    }

    $this->_traits = new Traits($this);

    return $this->_traits;

  }

  public function interfaces(): Interfaces {

    if ($this->_interfaces instanceof Interfaces) {
      return $this->_interfaces;
    }

    $this->_interfaces = new Interfaces($this);

    return $this->_interfaces;

  }

  public function inclusions(): Inclusions {

    if ($this->_inclusions instanceof Inclusions) {
      return $this->_inclusions;
    }

    $this->_inclusions = new Inclusions($this);

    return $this->_inclusions;

  }

  public function getLineToTokens(int $lineNo): Vector<TokenInterface> {
    $tokens = $this->_lineToTokens->get($lineNo);

    if ($tokens instanceof Vector) {
      return $tokens;
    }

    return Vector {};

  }

  public function getLinesToTests(): Map<int, Vector<string>> {
    $data = Map {};

    // unroll the map, into a vector for ease of use.
    foreach ($this->_lineToTests as $lineNo => $testsMap) {
      $data->set($lineNo, $testsMap->keys());
    }

    return $data;
  }

  public function setLineToTest(int $lineNo, string $testId): void {

    $data = $this->_lineToTests->get($lineNo);

    if (!$data instanceof Map) {
      $data = Map {};
    }

    $data->set($testId, true);

    $this->_lineToTests->set($lineNo, $data);

  }

  public function lineToTestToArrayFormat(): array<int, array<string>> {
    $data = array();

    foreach ($this->_lineToTests as $lineNo => $testsMap) {
      $data[$lineNo] = array();
      foreach ($testsMap as $testId => $tested) {
        $data[$lineNo][] = $testId;
      }
    }

    return $data;
  }

  public function stream(): Stream {

    if ($this->_stream instanceof Stream) {
      return $this->_stream;
    }

    $stream = new Stream($this);

    $scanner = new StreamScanner();
    $scanner->scan($this, $stream);
    $this->_stream = $stream;

    $parser = new StreamParser();
    $parser->parse($this, $stream);

    $this->_stream = $stream;
    return $this->_stream;

  }

  private function isTokenExecutable(
    TokenInterface $token,
    int $currentLine,
  ): (bool, int, string) {
    if ($token instanceof PHP_Token_Return) {
      $skipAmount = 0; // JEO: @TODO: I think there might be a issue for multi line return.
      return tuple(true, $skipAmount, 'return');
    }
    if ($token instanceof PHP_Token_Variable) {
      $skipAmount = 0;
      return tuple(true, $skipAmount, 'variable');
    }

    if ($token instanceof PHP_Token_Throw) {
      $skipAmount = $token->getEndOfDefinitionLineNo() - $currentLine;
      return tuple(true, $skipAmount, 'throw');
    }

    $skipAmount = 0;
    return tuple(false, $skipAmount, '');

  }

  private function isTokenComment(TokenInterface $token): bool {
    return false;
  }

  private function determineLineExecutableForeachLine(
    Vector<TokenInterface> $lineStack,
    int $currentLine,
  ): (bool, int, string) {

    $isExecutable = false;

    foreach ($lineStack as $token) {

      if ($token instanceof PHP_Token_Class) {
        $skipAmount = $token->getEndOfDefinitionLineNo() - $currentLine;
        return tuple(false, $skipAmount, 'class');
      }

      if ($token instanceof PHP_Token_Function) {
        $skipAmount = $token->getEndOfDefinitionLineNo() - $currentLine;
        return tuple(false, $skipAmount, 'function');
      }

      list($isExecutable, $skipAmount, $reason) =
        $this->isTokenExecutable($token, $currentLine);

      if ($isExecutable === true) {
        // we found a executable token, stop working so hard
        return tuple($isExecutable, $skipAmount, $reason);
      }

      if ($this->isTokenComment($token) === true) {
        // everything to the right of this token is useless
        break;
      }

    }

    return tuple($isExecutable, 0, '');

  }

  private function determineLineExecutable(
    Vector<TokenInterface> $lineStack,
    int $currentLine,
  ): int {

    // echo "execFile=".$this->_file." currentLine=$currentLine\n";

    $isExecutable = false;
    $skipAmount = 0;
    $reason = '';

    if ($lineStack->count() == 0) {
      // empty stack therefor noop
    } else if ($lineStack->count() == 1) {
      // if it's a trailing } from either a control structure it's a non-op
      $token = $lineStack->get(0);
      if ($token instanceof PHP_Token_Close_Curly) {
        $isExecutable = false;
      }
    } else {
      // Loop across the line looking for more complex patterns.
      list($isExecutable, $skipAmount, $reason) =
        $this->determineLineExecutableForeachLine($lineStack, $currentLine);
    }

    // mark the line up with the state.
    if ($isExecutable === true) {
      // echo " lineIsExecutable=".$currentLine."\n";
      if ($skipAmount > 0) {
        for ($s = 0; $s < $skipAmount; $s++) {
          $this->lineExecutionState()
            ->set(($currentLine + $s), Driver::LINE_NOT_EXECUTED);
        }
        $this->lineExecutionState()->addExecutableRange(
          $reason,
          $currentLine,
          $currentLine + $skipAmount,
        );
      } else {
        $this->lineExecutionState()
          ->set($currentLine, Driver::LINE_NOT_EXECUTED);
      }
    } else {
      $this->lineExecutionState()
        ->set($currentLine, Driver::LINE_NOT_EXECUTABLE);
    }

    return $skipAmount;

  }

  public function init(): void {

    if ($this->_didInit === true) {
      return;
    }

    $this->source()->load();
    $this->rawTokens()->load();
    // $this->stream();

    $tokenStream = $this->stream();
    $tokens = $tokenStream->tokens();

    $lineNo = -1;

    $tokenCount = $tokens->count();
    $lineStack = Vector {};
    $skipAmount = 0;

    for ($tokenOffset = 0; $tokenOffset < $tokenCount; $tokenOffset++) {

      $token = $tokens->get($tokenOffset);

      // not a token, prevent overrun jic we are stupid.
      if (!$token instanceof TokenInterface) {
        continue;
      }

      $nextLineNo = $token->getLine();

      if ($lineNo != $nextLineNo) {

        if ($lineNo === -1) {
          // do nothing
          $this->_startLine = $nextLineNo;
        } else {

          if ($skipAmount > 0) {
            $skipAmount--;
          } else {
            $skipAmount = $this->determineLineExecutable($lineStack, $lineNo);
          }

        }

        $this->_lineToTokens->set($lineNo, $lineStack);

        $lineStack->clear();

        $lineNo = $nextLineNo;

      }

      $lineStack->add($token);

    }

    if ($lineStack->count() !== 0) {
      $this->determineLineExecutable($lineStack, $lineNo);
    }

    $this->_endLine = $lineNo;
    $this->_didInit = true;

  }

  public function getStartLine(): int {
    return $this->_startLine;
  }

  public function getEndLine(): int {
    return $this->_endLine;
  }

}
