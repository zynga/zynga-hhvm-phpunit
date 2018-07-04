#!/usr/local/bin/hhvm
<?hh

$vendorDir = dirname(dirname(dirname(dirname(__FILE__))));
$projectRoot = dirname($vendorDir);

require_once $vendorDir . '/autoload.php';

use Zynga\PHPUnit\RunSingleTest;

$testRunner = new RunSingleTest(
  $projectRoot,
  $argv
);

$testRunner->run();
