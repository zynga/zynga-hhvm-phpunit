<?hh

namespace Zynga\PHPUnit\V2\Tests\Mock;

class Singleton {
  private static $uniqueInstance = null;

  protected function __construct() {}

  final private function __clone() {}

  public static function getInstance() {
    if (self::$uniqueInstance === null) {
      self::$uniqueInstance = new self();
    }

    return self::$uniqueInstance;
  }
}
