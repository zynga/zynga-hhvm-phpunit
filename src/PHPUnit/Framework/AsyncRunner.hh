<?hh

namespace PHPUnit\Framework;

use \PHPUnit_Framework_Test;
use \PHPUnit_Framework_TestResult;
use \PHPUnit_Framework_TestCase;
use Zynga\Framework\Testing\TestCase\V2\Base as ZyngaTestCaseBase;

//use \Awaitable;

class AsyncRunner {

 public static async function runTests(array $testStack): Awaitable<void> {
   //var_dump($testStack);
   await \HH\Asio\v($testStack);
 }

 public static async function runTestAsync(
     PHPUnit_Framework_Test $test,
     PHPUnit_Framework_TestResult $result):
     Awaitable<void> {

     /*
     if (($test instanceof ZyngaTestCaseBase || $test instanceof PHPUnit_Framework_TestCase )||
         $test instanceof self) {
         $test->setbeStrictAboutChangesToGlobalState($this->beStrictAboutChangesToGlobalState);
         $test->setBackupGlobals($this->backupGlobals);
         $test->setBackupStaticAttributes($this->backupStaticAttributes);
         $test->setRunTestInSeparateProcess($this->runTestInSeparateProcess);
     }
     */

     $test->run($result);

   }
 }

//class AsyncRunner {
