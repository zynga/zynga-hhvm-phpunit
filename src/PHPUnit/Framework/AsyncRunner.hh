<?hh

namespace PHPUnit\Framework;

use PHPUnit\Interfaces\TestInterface;

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
     TestInterface $test,
     PHPUnit_Framework_TestResult $result):
     Awaitable<void> {

     $test->run($result);

   }
 }

//class AsyncRunner {
