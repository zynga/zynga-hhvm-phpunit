<?hh // strict

namespace Zynga\PHPUnit\V2\Environment;

use Zynga\PHPUnit\V2\TestCase\OutputBuffer;

class OutputBuffering {
  private static bool $_isActive = false;

  public static function getCurrentLevel(): int {
    return ob_get_level();
  }

  public static function getIsActive(): bool {
    return self::$_isActive;
  }

  public static function start(?OutputBuffer $buffer = null): bool {

    // var_dump('ob start called currentLevel='.self::getCurrentLevel());

    if ($buffer instanceof OutputBuffer) {
      // --
      // Wire up the callback and set it to 1 byte in order to capture the call point
      // that did the underlying output.
      // --
      ob_start(array($buffer, 'outputCallback'), 1);
    } else {
      // let the og_call start up
      ob_start();
    }

    self::$_isActive = true;

    return true;

  }

  public static function end(): bool {

    // end and clean the buffer
    ob_end_clean();

    // refresh currentLevel as it should of decremented.
    $currentLevel = self::getCurrentLevel();

    if ($currentLevel == 0) {
      self::$_isActive = false;
    }

    return true;
  }

  public static function get(): string {

    $contents = ob_get_contents();

    if (is_string($contents)) {
      return $contents;
    }

    return '';

  }

}
