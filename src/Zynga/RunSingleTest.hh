<?hh

namespace Zynga;

class RunSingleTest {
  
  public function usage($message) {
    echo "Error: $message\n";
    echo "\n";
    echo "run-single-test.hh <path>Test.hh [testFunction]\n";
    echo "\n";
    echo " Runs a single test suite handles the phpunit arguments\n";
    echo "\n";
    exit(255);
  }

}
