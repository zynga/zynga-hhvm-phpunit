<?hh // strict

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace PHPUnit\Interfaces;

use \Exception;

use PHPUnit\Exceptions\AssertionFailedError;
use PHPUnit\Exceptions\Warning;
use PHPUnit\Interfaces\TestInterface;
use PHPUnit\Interfaces\TestSuiteInterface;

use \PHPUnit_Framework_TestSuite;

/**
 * A Listener for test progress.
 *
 * @since      Interface available since Release 2.0.0
 */
interface TestListener
{
    /**
     * An error occurred.
     *
     * @param TestInterface $test
     * @param Exception              $e
     * @param float                  $time
     */
    public function addError(TestInterface $test, Exception $e, float $time): void;

    /**
     * A warning occurred.
     *
     * @param TestInterface    $test
     * @param Warning $e
     * @param float                     $time
     *
     * @since Method available since Release 6.0.0
     *
     * @todo  Uncomment in time for PHPUnit 6.0.0
     *
     * @see   https://github.com/sebastianbergmann/phpunit/pull/1840#issuecomment-162535997
     */
//  public function addWarning(TestInterface $test, Warning $e, $time);

    /**
     * A failure occurred.
     *
     * @param TestInterface $test
     * @param AssertionFailedError $e
     * @param float $time
     */
    public function addFailure(TestInterface $test, AssertionFailedError $e, float $time): void;

    /**
     * Incomplete test.
     *
     * @param TestInterface $test
     * @param Exception              $e
     * @param float                  $time
     */
    public function addIncompleteTest(TestInterface $test, Exception $e, float $time): void;

    /**
     * Risky test.
     *
     * @param TestInterface $test
     * @param Exception              $e
     * @param float                  $time
     *
     * @since Method available since Release 4.0.0
     */
    public function addRiskyTest(TestInterface $test, Exception $e, float $time): void;

    /**
     * Skipped test.
     *
     * @param TestInterface $test
     * @param Exception              $e
     * @param float                  $time
     *
     * @since Method available since Release 3.0.0
     */
    public function addSkippedTest(TestInterface $test, Exception $e, float $time): void;

    /**
     * A test suite started.
     *
     * @param PHPUnit_Framework_TestSuite $suite
     *
     * @since Method available since Release 2.2.0
     */
    public function startTestSuite(TestSuiteInterface $suite): void;

    /**
     * A test suite ended.
     *
     * @param PHPUnit_Framework_TestSuite $suite
     *
     * @since Method available since Release 2.2.0
     */
    public function endTestSuite(TestSuiteInterface $suite): void;

    /**
     * A test started.
     *
     * @param TestInterface $test
     */
    public function startTest(TestInterface $test): void;

    /**
     * A test ended.
     *
     * @param TestInterface $test
     * @param float                  $time
     */
    public function endTest(TestInterface $test, float $time): void;
}
