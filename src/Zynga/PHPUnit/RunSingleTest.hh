<?hh

namespace Zynga\PHPUnit;

class RunSingleTest {
  private string $projectRoot;
  private array $argv;

  public function __construct(string $projectRoot, array $argv) {
    $this->projectRoot = $projectRoot;
    $this->argv = $argv;
  }

  public function usage($message): void {
    echo "Error: $message\n";
    echo "\n";
    echo "run-single-test.hh <path>Test.hh [testFunction]\n";
    echo "\n";
    echo " Runs a single test suite handles the phpunit arguments\n";
    echo "\n";
    exit(255);
  }

  public function run(): void {

    $filePath = null;
    $testFunction = '';
    $testName = '';

    if ( isset($this->argv[1]) ) {
      $filePath = $this->argv[1];
    } else {
      $this->usage('No test file path provided');
    }

    if ( isset($this->argv[2]) ) {
      $testFunction = $this->argv[2];
    }

    $pregs = array();
    if ( preg_match( '/\/([a-zA-Z0-9]*Test)\.hh$/', $filePath, $pregs) ) {
      $testName = $pregs[1];
    } else {
      $this->usage("Expecting a path ending in <TestName>Test.hh");
    }

    $command = array();
    $command[] = $this->projectRoot . '/vendor/bin/phpunit';
    $command[] = '--verbose';
    $command[] = '--debug';

    // add filter to get only a single function
    if ( $testFunction != '' ) {
      $command[] = '--filter \'/::' . $testFunction . '$/\'';
    }

    $command[] = $testName;
    $command[] = $filePath;

    $cmd = implode(' ', $command);

    echo "command: $cmd\n";
    passthru($cmd);

  }

}
