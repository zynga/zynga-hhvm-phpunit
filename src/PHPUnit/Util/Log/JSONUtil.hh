<?hh // strict

namespace PHPUnit\Util\Log;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use PHPUnit\Exceptions\AssertionFailedError;
use PHPUnit\Exceptions\Warning;
use PHPUnit\Framework\TestFailure;
use PHPUnit\Interfaces\TestInterface;
use PHPUnit\Interfaces\TestSuiteInterface;
use PHPUnit\Interfaces\TestListener;
use PHPUnit\Util\FilterUtil;
use PHPUnit\Util\PrinterUtil;
use PHPUnit\Util\StringUtil;
use PHPUnit\Util\TestUtil;
use \Exception;

/**
 * A TestListener that generates JSON messages.
 *
 * @since Class available since Release 3.0.0
 */
class JSONUtil extends PrinterUtil implements TestListener {

    /**
     * @var string
     */
    protected string $currentTestSuiteName = '';

    /**
     * @var string
     */
    protected string $currentTestName = '';

    /**
     * @var bool
     */
    protected bool $currentTestPass = true;

    /**
     * An error occurred.
     *
     * @param PHPUnit_Framework_Test $test
     * @param Exception              $e
     * @param float                  $time
     */
    public function addError(TestInterface $test, Exception $e, float $time): void {
        $this->writeCase(
            'error',
            $time,
            FilterUtil::getFilteredStacktrace($e),
            TestFailure::exceptionToString($e),
            $test
        );

        $this->currentTestPass = false;
    }

    /**
     * A warning occurred.
     *
     * @param PHPUnit_Framework_Test    $test
     * @param Warning $e
     * @param float                     $time
     *
     * @since Method available since Release 5.1.0
     */
    public function addWarning(TestInterface $test, Warning $e, float $time): void {
        $this->writeCase(
            'warning',
            $time,
            FilterUtil::getFilteredStacktrace($e),
            TestFailure::exceptionToString($e),
            $test
        );

        $this->currentTestPass = false;
    }

    /**
     * A failure occurred.
     *
     * @param PHPUnit_Framework_Test                 $test
     * @param AssertionFailedError $e
     * @param float                                  $time
     */
    public function addFailure(TestInterface $test, AssertionFailedError $e, float $time): void {
        $this->writeCase(
            'fail',
            $time,
            FilterUtil::getFilteredStacktrace($e),
            TestFailure::exceptionToString($e),
            $test
        );

        $this->currentTestPass = false;
    }

    /**
     * Incomplete test.
     *
     * @param PHPUnit_Framework_Test $test
     * @param Exception              $e
     * @param float                  $time
     */
    public function addIncompleteTest(TestInterface $test, Exception $e, float $time): void {
        $this->writeCase(
            'error',
            $time,
            FilterUtil::getFilteredStacktrace($e),
            'Incomplete Test: ' . $e->getMessage(),
            $test
        );

        $this->currentTestPass = false;
    }

    /**
     * Risky test.
     *
     * @param PHPUnit_Framework_Test $test
     * @param Exception              $e
     * @param float                  $time
     *
     * @since Method available since Release 4.0.0
     */
    public function addRiskyTest(TestInterface $test, Exception $e, float $time): void {
        $this->writeCase(
            'error',
            $time,
            FilterUtil::getFilteredStacktrace($e),
            'Risky Test: ' . $e->getMessage(),
            $test
        );

        $this->currentTestPass = false;
    }

    /**
     * Skipped test.
     *
     * @param PHPUnit_Framework_Test $test
     * @param Exception              $e
     * @param float                  $time
     */
    public function addSkippedTest(TestInterface $test, Exception $e, float $time): void {
        $this->writeCase(
            'error',
            $time,
            FilterUtil::getFilteredStacktrace($e),
            'Skipped Test: ' . $e->getMessage(),
            $test
        );

        $this->currentTestPass = false;
    }

    /**
     * A testsuite started.
     *
     * @param PHPUnit_Framework_TestSuite $suite
     */
    public function startTestSuite(TestSuiteInterface $suite): void {

        $this->currentTestSuiteName = $suite->getName();
        $this->currentTestName      = '';

        $this->write( Map {
            'event' => 'suiteStart',
            'suite' => $this->currentTestSuiteName,
            'tests' => count($suite)
          });
    }

    /**
     * A testsuite ended.
     *
     * @param PHPUnit_Framework_TestSuite $suite
     */
    public function endTestSuite(TestSuiteInterface $suite): void  {
      $this->currentTestSuiteName = '';
      $this->currentTestName      = '';
    }

    /**
     * A test started.
     *
     * @param PHPUnit_Framework_Test $test
     */
    public function startTest(TestInterface $test): void {

        $desc = TestUtil::describe($test);

        if ( is_string($desc) ) {
          $this->currentTestName = $desc;
        }

        $this->currentTestPass = true;

        $this->write(Map {
            'event' => 'testStart',
            'suite' => $this->currentTestSuiteName,
            'test'  => $this->currentTestName
          });

    }

    /**
     * A test ended.
     *
     * @param PHPUnit_Framework_Test $test
     * @param float                  $time
     */
    public function endTest(TestInterface $test, float $time): void {
      if ($this->currentTestPass) {
        $this->writeCase('pass', $time, '', '', $test);
      }
    }

    /**
     * @param string                          $status
     * @param float                           $time
     * @param array                           $trace
     * @param string                          $message
     * @param TestInterface|null $test
     */
    protected function writeCase(string $status, float $time, string $trace = '', string $message = '', ?TestInterface $test = null): void {

        $output = '';

        // take care of TestSuite producing error (e.g. by running into exception) as TestSuite doesn't have hasOutput
        if ($test instanceof TestInterface && $test->hasOutput()) {
            $output = $test->getActualOutput();
        }

        $this->write(Map {
            'event'   => 'test',
            'suite'   => $this->currentTestSuiteName,
            'test'    => $this->currentTestName,
            'status'  => $status,
            'time'    => $time,
            'trace'   => $trace,
            'message' => StringUtil::convertToUtf8($message),
            'output'  => $output,
          });
    }

    /**
     * @param string $buffer
     */
    public function writeHook(Map<string, mixed> $buffer): string {

      return json_encode($buffer, JSON_PRETTY_PRINT);

    }

}
