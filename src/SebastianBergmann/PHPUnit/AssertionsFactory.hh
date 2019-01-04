<?hh // strict

namespace SebastianBergmann\PHPUnit;

use SebastianBergmann\PHPUnit\Asssertions;

class AssertionsFactory {
  private static ?Assertions $assertions;

  public static function factory(): Assertions {

    if (self::$assertions instanceof Assertions) {
      return self::$assertions;
    }

    self::$assertions = new Assertions();

    return self::$assertions;

  }

}
