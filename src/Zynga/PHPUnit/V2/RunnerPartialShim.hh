<?hh // partial

namespace Zynga\PHPUnit\V2;

use \PHPUnit_TextUI_Command;

class RunnerPartialShim {
  public static function runPHPUnit(bool $return, array $argv): int {
    echo date('r')." - runPHPUnit args\n";
    var_dump($argv);
    return PHPUnit_TextUI_Command::main(false, $argv);
  }
}
