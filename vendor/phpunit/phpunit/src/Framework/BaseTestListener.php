<?hh

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use PHPUnit\Exceptions\Warning;
use PHPUnit\Interfaces\TestListener;

use \Exception;

use PHPUnit\Exceptions\AssertionFailedError;
use PHPUnit\Interfaces\TestInterface;
use PHPUnit\Interfaces\TestSuiteInterface;

use \PHPUnit_Framework_Test;

/**
 * An empty Listener that can be extended to implement TestListener
 * with just a few lines of code.
 *
 * @see TestListener for documentation on the API methods.
 * @since Class available since Release 4.0.0
 */
abstract class PHPUnit_Framework_BaseTestListener implements TestListener
{
    public function addError(TestInterface $test, Exception $e, float $time): void
    {
    }

    public function addWarning(TestInterface $test, Warning $e, float $time): void
    {
    }

    public function addFailure(TestInterface $test, AssertionFailedError $e, float $time): void
    {
    }

    public function addIncompleteTest(TestInterface $test, Exception $e, float $time): void
    {
    }

    public function addRiskyTest(TestInterface $test, Exception $e, float $time): void
    {
    }

    public function addSkippedTest(TestInterface $test, Exception $e, float $time): void
    {
    }

    public function startTestSuite(TestSuiteInterface $suite): void
    {
    }

    public function endTestSuite(TestSuiteInterface $suite): void
    {
    }

    public function startTest(TestInterface $test): void
    {
    }

    public function endTest(TestInterface $test, float $time): void {
    }

}
