<?hh // strict

namespace Zynga\PHPUnit\V2\Tests\System;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use Zynga\PHPUnit\V2\Interfaces\TestInterface;
use Zynga\PHPUnit\V2\Interfaces\TestListenerInterface;
use Zynga\PHPUnit\V2\TestCase;
use Zynga\PHPUnit\V2\Tests\Mock\Failure;
use Zynga\PHPUnit\V2\Tests\Mock\Success;
use Zynga\PHPUnit\V2\Tests\Mock\TestError;
use Zynga\PHPUnit\V2\TestResult;

use \Exception;

/**
 * @since      Class available since Release 2.0.0
 * @covers     TestCase
 */
class TestListenerTest extends TestCase implements TestListenerInterface {
  protected int $endCount = 0;
  protected int $errorCount = 0;
  protected int $failureCount = 0;
  protected int $warningCount = 0;
  protected int $notImplementedCount = 0;
  protected int $riskyCount = 0;
  protected int $skippedCount = 0;
  protected ?TestResult $result = null;
  protected int $startCount = 0;

  public function addError(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {
    $this->errorCount++;
  }

  public function addWarning(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {
    $this->warningCount++;
  }

  public function addFailure(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {
    $this->failureCount++;
  }

  public function addIncompleteTest(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {
    $this->notImplementedCount++;
  }

  public function addRiskyTest(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {
    $this->riskyCount++;
  }

  public function addSkippedTest(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {
    $this->skippedCount++;
  }

  public function startTestSuite(TestInterface $suite): void {}

  public function endTestSuite(TestInterface $suite): void {}

  public function startTest(TestInterface $test): void {
    $this->startCount++;
  }

  public function endTest(TestInterface $test, float $time): void {
    $this->endCount++;
  }

  public function setUp(): void {

    $result = new TestResult();
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
    $test = new TestError('testError');

    $test->run($this->result);

    $this->assertEquals(1, $this->errorCount, 'errorCount should be 1');
    $this->assertEquals(1, $this->endCount, 'endCount should be 1');
  }

  public function testFailure(): void {
    $test = new Failure('testFailure');
    $test->run($this->result);

    $this->assertEquals(1, $this->failureCount);
    $this->assertEquals(1, $this->endCount);
  }

  public function testStartStop(): void {
    $test = new Success('testNoop');
    $test->run($this->result);

    $this->assertEquals(1, $this->startCount);
    $this->assertEquals(1, $this->endCount);
  }

  public function flush(): void {}

}
