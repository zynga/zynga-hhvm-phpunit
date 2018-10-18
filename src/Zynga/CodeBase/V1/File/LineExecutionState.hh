<?hh // strict

namespace Zynga\CodeBase\V1\File;

use SebastianBergmann\CodeCoverage\Driver;
use Zynga\CodeBase\V1\File;
use Zynga\CodeBase\V1\File\ExecutableRange;

class LineExecutionState {
  private File $_parent;
  private Map<int, int> $_lineExecutionState;
  private Map<int, Vector<ExecutableRange>> $_executableRanges;

  public function __construct(File $parent) {
    $this->_parent = $parent;
    $this->_lineExecutionState = Map {};
    $this->_executableRanges = Map {};
  }

  public function isLineWithinExecutableRange(int $lineNo): bool {
    if ($this->_executableRanges->containsKey($lineNo) === true) {
      return true;
    }
    return false;
  }

  public function getExecutableRanges(int $lineNo): Vector<ExecutableRange> {
    $ranges = $this->_executableRanges->get($lineNo);
    if ( $ranges instanceof Vector ) {
      return $ranges;
    }
    return Vector {};
  }

  public function addFiniteExecutableRange(
    string $reason,
    int $lineNo,
    int $rangeStart,
    int $rangeEnd
  ): bool {
    $executableRange = new ExecutableRange($reason, $rangeStart, $rangeEnd);
    return $this->addRangeToLineNo($lineNo, $executableRange);
  }

  public function addRangeToLineNo(int $lineNo, ExecutableRange $range): bool {

    $currentRanges = $this->_executableRanges->get($lineNo);

    if ( ! $currentRanges instanceof Vector ) {
      $currentRanges = Vector {};
    }

    $currentRanges->add($range);

    $this->_executableRanges->set($lineNo, $currentRanges);

    return true;

  }

  public function addExecutableRange(
    string $reason,
    int $startLine,
    int $endLine,
  ): bool {


    $executableRange = new ExecutableRange($reason, $startLine, $endLine);

    for ($lineNo = $startLine; $lineNo <= $endLine; $lineNo++) {
      $this->addRangeToLineNo($lineNo, $executableRange);
    }

    return true;

  }

  public function get(int $lineNo): ?int {
    return $this->_lineExecutionState->get($lineNo);
  }

  public function set(int $lineNo, int $lineState): void {

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

    // after init is called, we don't let you go backwards to set not
    //   exec | not executed
    // if ($this->_didInit === true &&
    //     ($lineState === Driver::LINE_NOT_EXECUTABLE ||
    //      $lineState === Driver::LINE_NOT_EXECUTED)) {
    //   return;
    // }

    $currentValue = $this->_lineExecutionState->get($lineNo);

    if ($currentValue === null) {
      // no value at all for the stack.
      $this->_lineExecutionState->set($lineNo, $lineState);
      return;
    }

    if ($currentValue === Driver::LINE_NOT_EXECUTABLE) {
      // Line has been marked as non-executable.
      return;
    }

    if ($currentValue > $lineState) {
      error_log(
        sprintf(
          'ccFileWarning-attemptLineStateDegredation file=%s line=%d currentValue=%d lineState=%d',
          $this->_parent->getFile(),
          $lineNo,
          $currentValue,
          $lineState,
        ),
      );
      return;
    }

    $this->_lineExecutionState->set($lineNo, $lineState);

  }

  public function getAll(): Map<int, int> {
    return $this->_lineExecutionState;
  }

}
