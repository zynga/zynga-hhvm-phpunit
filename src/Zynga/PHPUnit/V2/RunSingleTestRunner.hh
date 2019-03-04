<?hh // strict

namespace Zynga\PHPUnit\V2;

use Zynga\PHPUnit\V2\Runner;

class RunSingleTestRunner extends Runner {

  public function scriptName(): string {
    return strval($this->argv->get(0));
  }

  public function usage(string $message): void {
    echo "Error: $message\n";
    echo "\n";
    echo $this->scriptName()." <path>Test.hh [testFunction]\n";
    echo "\n";
    echo " Runs a single test suite handles the phpunit arguments\n";
    echo "\n";
    exit(255);
  }

  public function createPHPUnitArgStack(): Vector<string> {

    if (!$this->argv->containsKey(1)) {
      $this->usage('No test file path provided');
    }

    $filePath = strval($this->argv->get(1));

    $testFunction = '';
    if ($this->argv->containsKey(2)) {
      $testFunction = strval($this->argv->get(2));
    }

    $testName = '';
    $pregs = array();
    if (preg_match('/\/([a-zA-Z0-9]*Test)\.hh$/', $filePath, $pregs)) {
      $testName = $pregs[1];
    } else {
      $this->usage("Expecting a path ending in <TestName>Test.hh");
    }

    $argStack = parent::createPHPUnitArgStack();

    echo date('r')." -   testName=$testName\n";
    echo date('r')." -   filePath=$filePath\n";

    if ($testFunction !== '') {
      echo date('r')." -   testFunction=$testFunction\n";
      $argStack->pop();
      $argStack->pop();
      $argStack->add('--debug');
      $argStack->add('--filter='.$testFunction);
      $argStack->add($filePath);
    } else {
      $argStack->add('--debug');
      $argStack->add($testName);
      $argStack->add($filePath);
    }

    return $argStack;

  }

}
