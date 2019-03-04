<?hh // strict

namespace Zynga\PHPUnit\V2\Interfaces;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use Zynga\PHPUnit\V2\Interfaces\TestInterface;
use \Exception;

/**
 * A Listener for test progress.
 *
 * @since      Interface available since Release 2.0.0
 */
interface TestListenerInterface {
  /**
   * An error occurred.
   *
   * @param TestCase $test
   * @param Exception              $e
   * @param float                  $time
   */
  public function addError(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void;

  /**
   * A warning occurred.
   *
   * @param TestCase    $test
   * @param Exception $e
   * @param float                     $time
   *
   * @since Method available since Release 6.0.0
   */
  public function addWarning(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void;

  /**
   * A failure occurred.
   *
   * @param TestCase                 $test
   * @param Exception $e
   * @param float                                  $time
   */
  public function addFailure(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void;

  /**
   * Incomplete test.
   *
   * @param TestInterface $test
   * @param Exception              $e
   * @param float                  $time
   */
  public function addIncompleteTest(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void;

  /**
   * Risky test.
   *
   * @param TestCase $test
   * @param Exception              $e
   * @param float                  $time
   *
   * @since Method available since Release 4.0.0
   */
  public function addRiskyTest(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void;

  /**
   * Skipped test.
   *
   * @param TestCase $test
   * @param Exception              $e
   * @param float                  $time
   *
   * @since Method available since Release 3.0.0
   */
  public function addSkippedTest(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void;

  /**
   * A test suite started.
   *
   * @param PHPUnit_Framework_TestSuite $suite
   *
   * @since Method available since Release 2.2.0
   */
  public function startTestSuite(TestInterface $suite): void;

  /**
   * A test suite ended.
   *
   * @param PHPUnit_Framework_TestSuite $suite
   *
   * @since Method available since Release 2.2.0
   */
  public function endTestSuite(TestInterface $suite): void;

  /**
   * A test started.
   *
   * @param TestCase $test
   */
  public function startTest(TestInterface $test): void;

  /**
   * A test ended.
   *
   * @param TestCase $test
   * @param float                  $time
   */
  public function endTest(TestInterface $test, float $time): void;

  /**
   * Flushing of output can be accomplished via this method.
   */
  public function flush(): void;

}
