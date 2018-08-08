<?hh // strict

namespace SebastianBergmann\CodeCoverage\ProcessedFile;

use SebastianBergmann\CodeCoverage\Driver;
use SebastianBergmann\CodeCoverage\Driver\HHVM\ExecBlock;
use SebastianBergmann\CodeCoverage\Driver\HHVM\Logging as HHVM_Logging;
use SebastianBergmann\CodeCoverage\Driver\HHVM\CodeBlock\AbstractBlock;
use SebastianBergmann\CodeCoverage\Driver\HHVM\CodeBlock\CodeBlockInterface;
use SebastianBergmann\CodeCoverage\Driver\HHVM\CodeBlock\IfBlock;
use SebastianBergmann\CodeCoverage\Driver\HHVM\CodeBlock\ReturnBlock;
use SebastianBergmann\TokenStream\Token\Stream;
use SebastianBergmann\TokenStream\Token\Stream\CachingFactory;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Comment;

class ProcessedFile {
  private bool $_didInit;
  private string $_file;
  private Map<int, int> $_lineExecutionState;
  private Map<int, Map<string, bool>> $_lineToTests;
  private int $_startLine;
  private int $_endLine;

  public function __construct(string $file) {
    $this->_didInit = false;
    $this->_file = $file;
    $this->_lineExecutionState = Map {};
    $this->_lineToTests = Map {};
    $this->_startLine = -1;
    $this->_endLine = -1;
  }

  public function getFile(): string {
    return $this->_file;
  }

  public function getLineExecutionState(int $lineNo): ?int {
    return $this->getLineExecutionState($lineNo);
  }

  public function setLineExecutionState(int $lineNo, int $lineState): void {

    if ($lineState !== Driver::LINE_NOT_EXECUTABLE &&
        $lineState !== Driver::LINE_NOT_EXECUTED &&
        $lineState !== Driver::LINE_EXECUTED) {
      /*
       Logging::debug(
       'WARNING-INVALID-LINE_STATE '.
       'file='.
       $this->_file.
       'lineNo='.
       $lineNo.
       ' lineState='.
       $lineState,
       );
       */
      return;
    }

    $currentValue = $this->_lineExecutionState->get($lineNo);

    if ($currentValue === null) {
      // no value at all for the stack.
      $this->_lineExecutionState->set($lineNo, $lineState);
      return;
    }

    if ($currentValue > $lineState) {
      error_log(
        sprintf(
          'ccFileWarning-attemptLineStateDegredation file=%s line=%d currentValue=%d lineState=%d',
          $this->_file,
          $lineNo,
          $currentValue,
          $lineState,
        ),
      );
      return;
    }

    $this->_lineExecutionState->set($lineNo, $lineState);

  }

  public function getAllLineExecutionState(): Map<int, int> {
    return $this->_lineExecutionState;
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
    return CachingFactory::get($this->_file);
  }

  public function init(): void {

    $tokenStream = $this->stream();
    $tokens = $tokenStream->tokens();

    $lineNo = -1;
    $executable = false;
    foreach ($tokens as $token) {

      $nextLineNo = $token->getLine();

      if ($lineNo != $nextLineNo) {

        if ($lineNo === -1) {
          // do nothing
          $this->_startLine = $nextLineNo;
        } else {
          if ($executable === true) {
            $this->setLineExecutionState(
              ($lineNo - 1),
              Driver::LINE_NOT_EXECUTED,
            );
          } else {
            $this->setLineExecutionState(
              ($lineNo - 1),
              Driver::LINE_NOT_EXECUTABLE,
            );
          }
          $executable = false;
        }

        $lineNo = $nextLineNo;

      }

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

  private bool $statsCalculated = false;

  private int $numExecutableLines = -1;
  private int $numExecutedLines = -1;

  private int $numTraits = -1;
  private int $numTestedTraits = -1;

  private int $numClasses = -1;
  private int $numTestedClasses = -1;

  private int $numMethods = -1;
  private int $numTestedMethods = -1;

  private int $numFunctions = -1;
  private int $numTestedFunctions = -1;

  public function getNumExecutedableLines(bool $recalculate = false): int {
    $this->calculateStatistics($recalculate);
    return $this->numExecutableLines;
  }

  public function getNumExecutedLines(bool $recalculate = false): int {
    $this->calculateStatistics($recalculate);
    return $this->numExecutedLines;
  }

  public function getNumTraits(bool $recalculate = false): int {
    $this->calculateStatistics($recalculate);
    return $this->numTraits;
  }

  public function getNumTestedTraits(bool $recalculate = false): int {
    $this->calculateStatistics($recalculate);
    return $this->numTestedTraits;
  }

  public function getNumClasses(bool $recalculate = false): int {
    $this->calculateStatistics($recalculate);
    return $this->numClasses;
  }

  public function getNumTestedClasses(bool $recalculate = false): int {
    $this->calculateStatistics($recalculate);
    return $this->numTestedClasses;
  }

  public function getNumMethods(bool $recalculate = false): int {
    $this->calculateStatistics($recalculate);
    return $this->numMethods;
  }

  public function getNumTestedMethods(bool $recalculate = false): int {
    $this->calculateStatistics($recalculate);
    return $this->numTestedMethods;
  }

  public function getNumFunctions(bool $recalculate = false): int {
    $this->calculateStatistics($recalculate);
    return $this->numFunctions;
  }

  public function getNumTestedFunctions(bool $recalculate = false): int {
    $this->calculateStatistics($recalculate);
    return $this->numTestedFunctions;
  }

  public function calculateStatistics(bool $recalculate = false): void {

    // if we have already calculated the stats, don't bother running them again.
    if ($this->statsCalculated === true && $recalculate === false) {
      return;
    }

    $this->numExecutableLines = 0;
    $this->numExecutedLines = 0;

    $this->numTraits = 0;
    $this->numTestedTraits = 0;

    $this->numClasses = 0;
    $this->numTestedClasses = 0;

    $this->numMethods = 0;
    $this->numTestedMethods = 0;

    $this->numFunctions = 0;
    $this->numTestedFunctions = 0;

    $stream = $this->stream();

    foreach ($stream->getClasses() as $className => $classObj) {

      $this->numClasses++;
      $this->numExecutableLines += $classObj->executableLines;
      $this->numExecutedLines += $classObj->executedLines;

      $classObj->numMethods = 0;
      $classObj->numTestedMethods = 0;

      $classObj->calculateCoverage();

      foreach ($classObj->methods as $methodObj) {

        $this->numMethods++;
        $classObj->numMethods++;

        // this is a fully covered function
        if ($methodObj->coverage == 100) {
          $this->numTestedMethods++;
          $classObj->numTestedMethods++;
        }

      }

      if ($classObj->coverage == 100) {
        $this->numTestedClasses++;
      }

    }

    foreach ($stream->getTraits() as $traitName => $traitObj) {

      $this->numTraits++;
      $this->numExecutableLines += $traitObj->executableLines;
      $this->numExecutedLines += $traitObj->executedLines;

      $traitObj->numMethods = 0;
      $traitObj->numTestedMethods = 0;

      $traitObj->calculateCoverage();

      foreach ($traitObj->methods as $methodObj) {

        $this->numMethods++;
        $traitObj->numMethods++;

        // this is a fully covered function
        if ($methodObj->coverage == 100) {
          $this->numTestedMethods++;
          $traitObj->numTestedMethods++;
        }

      }

      if ($traitObj->coverage == 100) {
        $this->numTestedClasses++;
      }

    }

    foreach ($stream->getFunctions() as $functionName => $functionObj) {
      $this->numFunctions++;

      // Calculate coverage amount
      $functionObj->calculateCoverage();

      // this is a fully covered function
      if ($functionObj->coverage == 100) {
        $this->numTestedFunctions++;
      }

      $this->numExecutableLines += $functionObj->executableLines;
      $this->numExecutedLines += $functionObj->executedLines;

    }

    $this->statsCalculated = true;

  }

}
