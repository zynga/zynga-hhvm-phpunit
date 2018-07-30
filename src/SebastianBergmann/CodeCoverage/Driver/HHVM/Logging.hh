<?hh // strict

namespace SebastianBergmann\CodeCoverage\Driver\HHVM;

class Logging {
  public static bool $_debug = false;

  public static function debug(string $message): void {
    if (static::$_debug === true) {
      static::message($message);
    }
  }

  public static function message(string $message): void {
    echo $message."\n";
  }

}
