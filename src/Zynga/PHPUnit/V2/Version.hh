<?hh // strict

namespace Zynga\PHPUnit\V2;

// --
// We utilize superheros for our major releases:
// - 1.8.x : ajax
//
// --
class Version {
  const string VERSION_NUMBER = "1.8.7";
  const string VERSION_NAME   = "ajax";

  public static function get(): string {
    return self::VERSION_NUMBER . ' - ' . self::VERSION_NAME;
  }

}
