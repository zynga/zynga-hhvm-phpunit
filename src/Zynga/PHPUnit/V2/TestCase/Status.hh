<?hh // strict

namespace Zynga\PHPUnit\V2\TestCase;

class Status {
  const int STATUS_NOT_STARTED = -1;
  const int STATUS_PASSED = 0;
  const int STATUS_SKIPPED = 1;
  const int STATUS_INCOMPLETE = 2;
  const int STATUS_FAILURE = 3;
  const int STATUS_ERROR = 4;
  const int STATUS_RISKY = 5;
  const int STATUS_WARNING = 6;

  private string $message = '';
  private int $code = -1;

  public function setMessageAndCode(string $message, int $code): bool {
    if ($this->setCode($code) && $this->setMessage($message)) {
      return true;
    }
    return false;
  }

  public function setMessage(string $message): bool {
    // if (preg_match('/#runtime/', $message)) {
    //   var_dump('runtime-trap');
    //   var_dump(debug_backtrace(2));
    //   exit();
    // }
    $this->message = $message;
    return true;
  }

  public function getMessage(): string {
    return $this->message;
  }

  public function setCode(int $code): bool {
    $this->code = $code;
    return true;
  }

  public function getCode(): int {
    return $this->code;
  }

}
