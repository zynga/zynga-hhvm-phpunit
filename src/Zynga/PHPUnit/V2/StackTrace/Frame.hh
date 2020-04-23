<?hh // strict

namespace Zynga\PHPUnit\V2\StackTrace;

use Zynga\Framework\Environment\CodePath\V1\CodePath;
use Zynga\Framework\ReflectionCache\V1\ReflectionClasses;
use Zynga\Framework\ReflectionCache\V1\ReflectionMethods;
use \ReflectionClass;
use \ReflectionMethod;
use \Exception;

class Frame {

  private int $_line;
  private string $_file;
  private string $_class;
  private string $_function;

  private ?Exception $_exception;

  public function __construct() {
    $this->_line = -1;
    $this->_file = '';
    $this->_class = '';
    $this->_function = '';

    $this->_exception = null;

  }

  public function getLine(): int {
    return $this->_line;
  }

  public function getFile(): string {
    return $this->_file;
  }

  public function getClass(): string {
    return $this->_class;
  }

  public function getFunction(): string {
    return $this->_function;
  }

  public function getException(): ?Exception {
    return $this->_exception;
  }

  public function setException(Exception $e): bool {
    $this->_exception = $e;
    return true;
  }

  public function shortenFileName(string $fileName): string {
    // phpunit
    $file = str_replace(dirname(dirname(__FILE__)), '<PHPUNIT>', $fileName);

    // zynga framework
    $codeEnvReflected = ReflectionClasses::getReflection(CodePath::class);

    if ($codeEnvReflected instanceof ReflectionClass) {
      $pathToEnv = $codeEnvReflected->getFileName();

      $file = str_replace(
        dirname(dirname(dirname(dirname($pathToEnv)))),
        '<ZYNGA_FRAMEWORK>',
        $file,
      );
    }

    // our code root
    $file = str_replace(CodePath::getRoot(), '<CODE_ROOT>', $file);
    return $file;
  }

  public function getShortFileName(): string {
    return $this->shortenFileName($this->_file);
  }

  public function consumeRawFrame(array<string, mixed> $frame): bool {

    //var_dump($frame);
    //exit();

    if (array_key_exists('file', $frame)) {
      $this->_file = strval($frame['file']);
    }

    if (array_key_exists('line', $frame)) {
      $this->_line = intval($frame['line']);
    }

    if (array_key_exists('class', $frame)) {
      $this->_class = strval($frame['class']);
    }

    if (array_key_exists('function', $frame)) {
      $this->_function = strval($frame['function']);
    }

    // --
    // JEO: Dynamic method calls get lost in the stack unwind. This fixes that as we reflect the class
    //   and lineno back into the object.
    // --
    if ($this->_file == '' &&
        $this->_line == -1 &&
        $this->_class != '' &&
        $this->_function != '') {

      $reflectedMethod =
        ReflectionMethods::getReflection($this->_class, $this->_function);

      if ($reflectedMethod instanceof ReflectionMethod) {
        $this->_file = strval($reflectedMethod->getFileName());
        $this->_line = intval($reflectedMethod->getStartLine());
      }

    }

    return true;

  }

  public function isValidFrame(): bool {

    return true;

    if ($this->_file == '' && $this->_line == -1) {
      // The legacy dynamic calls can create a null frame, skip it.
      return false;
    }

    return true;

  }

}
