<?hh // strict

namespace SebastianBergmann\PHPUnit\Environment;

class OutputBuffering {
  private static bool $_isActive = false;

  public static function getCurrentLevel(): int {
    return ob_get_level();
  }

  public static function getIsActive(): bool {
    return self::$_isActive;
  }

  public static function start(): bool {

    ob_start();

    self::$_isActive = true;

    return true;

  }

  public static function end(): bool {

    ob_end_clean();

    if ( self::getCurrentLevel() == 0 ) {
      self::$_isActive = false;
    }

    return true;
  }

  public static function get(): string {

    return ob_get_contents();

  }

}
