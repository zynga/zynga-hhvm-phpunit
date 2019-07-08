<?hh // strict

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

/**
 * An empty Listener that can be extended to implement TestListener
 * with just a few lines of code.
 *
 * @see TestListenerInterface for documentation on the API methods.
 * @since Class available since Release 4.0.0
 */
abstract class PHPUnit_Framework_BaseTestListener
  implements TestListenerInterface {

  public function addError(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {}

  public function addWarning(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {}

  public function addFailure(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {}

  public function addIncompleteTest(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {}

  public function addRiskyTest(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {}

  public function addSkippedTest(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {}

  public function startTestSuite(TestInterface $suite): void {}

  public function endTestSuite(TestInterface $suite): void {}

  public function startTest(TestInterface $test): void {}

  public function endTest(TestInterface $test, float $time): void {}

  public function flush(): void {}

}
