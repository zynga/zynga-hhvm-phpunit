<?hh // strict

namespace PHPUnit\Util\ErrorHandlerUtil;

class CapturedError {
  public int    $errno = 0;
  public string $errstr = '';
  public string $errfile = '';
  public int    $errline = 0;

  public function __construct(int $errno, string $errstr, string $errfile, int $errline) {
    $this->errno = $errno;
    $this->errstr = $errstr;
    $this->errfile = $errfile;
    $this->errline = $errline;
  }

}
