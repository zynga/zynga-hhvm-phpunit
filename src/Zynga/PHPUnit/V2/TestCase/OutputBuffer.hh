<?hh // strict

namespace Zynga\PHPUnit\V2\TestCase;

use Zynga\PHPUnit\V2\Environment\OutputBuffering;
use Zynga\PHPUnit\V2\Exceptions\TestError\RiskyException;

class OutputBuffer {

  private int $_outputBufferingLevel = -1;
  private string $_output = '';
  private string $_file = '';
  private int $_line = -1;

  /**
   * @return string
   *
   * @since Method available since Release 3.6.0
   */
  public function getActualOutput(): string {
    return $this->_output;
  }

  public function getOutputFile(): string {
    return $this->_file;
  }

  public function getOutputLine(): int {
    return $this->_line;
  }

  /**
   * @since Method available since Release 4.2.0
   */
  public function startOutputBuffering(): bool {

    $bt = debug_backtrace(0, 5);
    // error_log('JEO outputbuffer::start bt='.json_encode($bt));

    OutputBuffering::start($this);

    $this->_outputBufferingLevel = OutputBuffering::getCurrentLevel();

    return true;

  }

  /**
   * @since Method available since Release 4.2.0
   */
  public function stopOutputBuffering(): bool {

    /*
    $bt = debug_backtrace(0, 5);
    error_log(
      'JEO'.
      ' outputbuffer::stop bt='.
      json_encode($bt).
      ' bufferingLevel='.
      OutputBuffering::getCurrentLevel().
      ' _savedLevel='.
      $this->_outputBufferingLevel,
    );
    */

    if (OutputBuffering::getCurrentLevel() != $this->_outputBufferingLevel) {
      throw new RiskyException(
        'Test code or tested code did not (only) close its own output buffers',
      );
    }

    if (!OutputBuffering::getIsActive()) {
      return false;
    }

    OutputBuffering::end();

    return true;

  }

  public function outputCallback(string $output): void {

    // no output, we dunna care =).
    if ($output == '') {
      return;
    }

    $this->_output .= $output;

    // have we already seen output for this level?
    if ($this->_file != '') {
      return;
    }

    // backtrace file/lineo please
    $bt = debug_backtrace(0, 5);

    // error_log('JEO bt='.json_encode($bt));

    if (!array_key_exists(0, $bt)) {
      return;
    }

    $frame = $bt[0];

    if (!array_key_exists('file', $frame) ||
        !array_key_exists('line', $frame)) {
      return;
    }

    $file = $frame['file'];
    $line = $frame['line'];

    $this->_file = $file;
    $this->_line = $line;

  }

}
