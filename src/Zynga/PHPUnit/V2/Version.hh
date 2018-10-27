<?hh // strict

namespace Zynga\PHPUnit\V2;

// --
// We utilize superheros for our major releases:
// - 1.8.x : ajax
// - 1.9.x : blade
// - 2.0.0 : cable
//
// --
class Version {
  const string VERSION_NUMBER = "2.0.5";
  const string VERSION_NAME = "cable";

  public static function get(): string {
    return self::VERSION_NUMBER.' - '.self::VERSION_NAME;
  }

}
