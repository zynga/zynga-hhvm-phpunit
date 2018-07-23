<?hh // partial

namespace Zynga\PHPUnit\V2;

use \PHPUnit_TextUI_Command;

class RunnerPartialShim {
  public static function runPHPUnit(bool $return, array $argv): int {
    return PHPUnit_TextUI_Command::main(false, $argv);
  }
}
