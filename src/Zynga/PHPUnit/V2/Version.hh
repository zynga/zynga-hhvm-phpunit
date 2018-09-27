<?hh // strict

namespace Zynga\PHPUnit\V2;

// --
// We utilize superheros for our major releases:
// - 1.8.x : ajax
//
// --
class Version {
  const string VERSION_NUMBER = "1.9.6";
  const string VERSION_NAME   = "blade";

  public static function get(): string {
    return self::VERSION_NUMBER . ' - ' . self::VERSION_NAME;
  }

}
