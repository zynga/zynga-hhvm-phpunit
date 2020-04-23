<?hh // strict

namespace Zynga\PHPUnit\V2\StackTrace;

use Zynga\PHPUnit\V2\StackTrace\Frame;
use \Exception;

class Stack {
  private Vector<Frame> $_frames;

  public function __construct() {
    $this->_frames = Vector {};
  }

  public function getFrames(): Vector<Frame> {
    return $this->_frames;
  }

  public function consumeException(Exception $e): void {

    $trace = $e->getTrace();

    $exceptionPointClass = 'UNKNOWN';
    $exceptionPointFunction = 'UNKNOWN';

    $firstFrame = null;

    if (is_array($trace[0])) {
      $firstFrame = $trace[0];
    }

    if (is_array($firstFrame) && array_key_exists('class', $firstFrame)) {
      $exceptionPointClass = $firstFrame['class'];
    }

    if (is_array($firstFrame) && array_key_exists('function', $firstFrame)) {
      $exceptionPointFunction = $firstFrame['function'];
    }

    $exceptionPoint = new Frame();

    $exceptionPoint->consumeRawFrame(
      array(
        'file' => $e->getFile(),
        'line' => $e->getLine(),
        'class' => $exceptionPointClass,
        'function' => $exceptionPointFunction,
      ),
    );

    $exceptionPoint->setException($e);

    $this->_frames->add($exceptionPoint);

    foreach ($trace as $rawFrame) {
      $file = '';

      $frame = new Frame();
      $frame->consumeRawFrame($rawFrame);

      if ($frame->isValidFrame() !== true) {
        continue;
      }

      $this->_frames->add($frame);

    }

  }

}
