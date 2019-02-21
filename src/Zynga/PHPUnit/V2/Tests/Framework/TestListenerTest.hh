<?hh // strict

namespace Zynga\PHPUnit\V2\Tests;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use Zynga\PHPUnit\V2\Interfaces\TestListenerInterface;
use Zynga\PHPUnit\V2\Tests\Mock\Failure;
use Zynga\PHPUnit\V2\Tests\Mock\Success;
use Zynga\PHPUnit\V2\Tests\Mock\TestError;

use \PHPUnit_Framework_Test;
use \PHPUnit_Framework_TestCase;
use \PHPUnit_Framework_TestResult;
use \PHPUnit_Framework_TestSuite;
use \Exception;

/**
 * @since      Class available since Release 2.0.0
 * @covers     PHPUnit_Framework_TestCase
 */
class TestListenerTest extends PHPUnit_Framework_TestCase
  implements TestListenerInterface {
  protected int $endCount = 0;
  protected int $errorCount = 0;
  protected int $failureCount = 0;
  protected int $warningCount = 0;
  protected int $notImplementedCount = 0;
  protected int $riskyCount = 0;
  protected int $skippedCount = 0;
  protected ?PHPUnit_Framework_TestResult $result = null;
  protected int $startCount = 0;

  public function addError(
    PHPUnit_Framework_Test $test,
    Exception $e,
    float $time,
  ): void {
    $this->errorCount++;
  }

  public function addWarning(
    PHPUnit_Framework_Test $test,
    Exception $e,
    float $time,
  ): void {
    $this->warningCount++;
  }

  public function addFailure(
    PHPUnit_Framework_Test $test,
    Exception $e,
    float $time,
  ): void {
    $this->failureCount++;
  }

  public function addIncompleteTest(
    PHPUnit_Framework_Test $test,
    Exception $e,
    float $time,
  ): void {
    $this->notImplementedCount++;
  }

  public function addRiskyTest(
    PHPUnit_Framework_Test $test,
    Exception $e,
    float $time,
  ): void {
    $this->riskyCount++;
  }

  public function addSkippedTest(
    PHPUnit_Framework_Test $test,
    Exception $e,
    float $time,
  ): void {
    $this->skippedCount++;
  }

  public function startTestSuite(PHPUnit_Framework_TestSuite $suite): void {}

  public function endTestSuite(PHPUnit_Framework_TestSuite $suite): void {}

  public function startTest(PHPUnit_Framework_Test $test): void {
    $this->startCount++;
  }

  public function endTest(PHPUnit_Framework_Test $test, float $time): void {
    $this->endCount++;
  }

  public function setUp(): void {

    $result = new PHPUnit_Framework_TestResult();
    $result->listeners()->add($this);

    $this->result = $result;
    $this->endCount = 0;
    $this->failureCount = 0;
    $this->notImplementedCount = 0;
    $this->riskyCount = 0;
    $this->skippedCount = 0;
    $this->startCount = 0;
  }

  public function testError(): void {
    $test = new TestError();
    $test->run($this->result);

    $this->assertEquals(1, $this->errorCount);
    $this->assertEquals(1, $this->endCount);
  }

  public function testFailure(): void {
    $test = new Failure();
    $test->run($this->result);

    $this->assertEquals(1, $this->failureCount);
    $this->assertEquals(1, $this->endCount);
  }

  public function testStartStop(): void {
    $test = new Success();
    $test->run($this->result);

    $this->assertEquals(1, $this->startCount);
    $this->assertEquals(1, $this->endCount);
  }

  public function flush(): void {}

}
