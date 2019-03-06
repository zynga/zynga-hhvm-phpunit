<?hh // strict

namespace Zynga\PHPUnit\V2\TestCase;

use SebastianBergmann\PHPUnit\Environment\OutputBuffering;
use SebastianBergmann\PHPUnit\Exceptions\TestError\RiskyException;

class OutputBuffer {

  private string $_output = '';
  private int $_outputBufferingLevel = -1;

  /**
   * @return string
   *
   * @since Method available since Release 3.6.0
   */
  public function getActualOutput(): string {
    return $this->_output;
  }

  /**
   * @since Method available since Release 4.2.0
   */
  public function startOutputBuffering(): bool {

    OutputBuffering::start();

    $this->_outputBufferingLevel = OutputBuffering::getCurrentLevel();

    return true;

  }

  /**
   * @since Method available since Release 4.2.0
   */
  public function stopOutputBuffering(): bool {

    // error_log('JEO bufferingLevel=' . OutputBuffering::getCurrentLevel() . ' _savedLevel=' . $this->_outputBufferingLevel);

    if (OutputBuffering::getCurrentLevel() != $this->_outputBufferingLevel) {
      throw new RiskyException(
        'Test code or tested code did not (only) close its own output buffers',
      );
    }

    if (!OutputBuffering::getIsActive()) {
      return false;
    }

    $output = OutputBuffering::get();

    if ( $output != '' ) {
      $this->_output = $output;
    }

    // error_log('JEO outputSaved len=' . strlen($this->_output) . ' output=' . $this->_output);

    OutputBuffering::end();

    $this->_outputBufferingLevel = OutputBuffering::getCurrentLevel();

    return true;

  }

}
