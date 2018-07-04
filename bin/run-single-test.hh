#!/usr/local/bin/hhvm
<?hh

$vendorDir = dirname(dirname(dirname(dirname(__FILE__))));
$projectRoot = dirname(dirname($vendorDir));

require_once $vendorDir . '/autoload.php';

use Zynga\RunSingleTest;

$testRunner = new RunSingleTest(
  $projectRoot
);

//$testRunner->run();


$filePath = null;
if ( isset($argv[1]) ) {
  $filePath = $argv[1];
} else {
  $runner->usage('No test file path provided');
}

$testFunction = '';

if ( isset($argv[2]) ) {
  $testFunction = $argv[2];
}

$testName = '';

if ( preg_match( '/\/([a-zA-Z0-9]*Test)\.hh$/', $filePath, $pregs) ) {
  $testName = $pregs[1];
} else {
  $runner->usage("Expecting a path ending in <TestName>Test.hh");
}

$command = array();
$command[] = $projectRoot . '/vendor/bin/phpunit';
$command[] = '--debug';

// add filter to get only a single function
if ( $testFunction != '' ) {
  $command[] = '--filter \'/::' . $testFunction . '$/\'';
}

$command[] = $testName;
$command[] = $filePath;

$cmd = join(' ', $command);

echo "command: $cmd\n";
passthru($cmd);
