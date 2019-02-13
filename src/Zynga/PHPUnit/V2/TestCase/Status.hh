<?hh // strict

namespace Zynga\PHPUnit\V2\TestCase;

class Status {
  private string $message = '';
  private int $code = 0;

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
