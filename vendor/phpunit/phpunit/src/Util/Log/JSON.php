<?php
/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use Zynga\PHPUnit\V2\Interfaces\TestListenerInterface;
use Zynga\PHPUnit\V2\TestCase;

use \Exception;

/**
 * A TestListener that generates JSON messages.
 *
 * @since Class available since Release 3.0.0
 */
class PHPUnit_Util_Log_JSON extends PHPUnit_Util_Printer implements TestListenerInterface
{
    /**
     * @var string
     */
    protected $currentTestSuiteName = '';

    /**
     * @var string
     */
    protected $currentTestName = '';

    /**
     * @var bool
     */
    protected $currentTestPass = true;

    /**
     * An error occurred.
     *
     * @param TestInterface $test
     * @param Exception              $e
     * @param float                  $time
     */
    public function addError(TestInterface $test, Exception $e, $time)
    {
        $this->writeCase(
            'error',
            $time,
            PHPUnit_Util_Filter::getFilteredStacktrace($e, false),
            PHPUnit_Framework_TestFailure::exceptionToString($e),
            $test
        );

        $this->currentTestPass = false;
    }

    /**
     * A warning occurred.
     *
     * @param TestInterface    $test
     * @param Exception $e
     * @param float                     $time
     *
     * @since Method available since Release 5.1.0
     */
    public function addWarning(TestInterface $test, Exception $e, $time)
    {
        $this->writeCase(
            'warning',
            $time,
            PHPUnit_Util_Filter::getFilteredStacktrace($e, false),
            PHPUnit_Framework_TestFailure::exceptionToString($e),
            $test
        );

        $this->currentTestPass = false;
    }

    /**
     * A failure occurred.
     *
     * @param TestInterface                 $test
     * @param Exception $e
     * @param float                                  $time
     */
    public function addFailure(TestInterface $test, Exception $e, $time)
    {
        $this->writeCase(
            'fail',
            $time,
            PHPUnit_Util_Filter::getFilteredStacktrace($e, false),
            PHPUnit_Framework_TestFailure::exceptionToString($e),
            $test
        );

        $this->currentTestPass = false;
    }

    /**
     * Incomplete test.
     *
     * @param TestInterface $test
     * @param Exception              $e
     * @param float                  $time
     */
    public function addIncompleteTest(TestInterface $test, Exception $e, $time)
    {
        $this->writeCase(
            'error',
            $time,
            PHPUnit_Util_Filter::getFilteredStacktrace($e, false),
            'Incomplete Test: ' . $e->getMessage(),
            $test
        );

        $this->currentTestPass = false;
    }

    /**
     * Risky test.
     *
     * @param TestInterface $test
     * @param Exception              $e
     * @param float                  $time
     *
     * @since Method available since Release 4.0.0
     */
    public function addRiskyTest(TestInterface $test, Exception $e, $time)
    {
        $this->writeCase(
            'error',
            $time,
            PHPUnit_Util_Filter::getFilteredStacktrace($e, false),
            'Risky Test: ' . $e->getMessage(),
            $test
        );

        $this->currentTestPass = false;
    }

    /**
     * Skipped test.
     *
     * @param TestInterface $test
     * @param Exception              $e
     * @param float                  $time
     */
    public function addSkippedTest(TestInterface $test, Exception $e, $time)
    {
        $this->writeCase(
            'error',
            $time,
            PHPUnit_Util_Filter::getFilteredStacktrace($e, false),
            'Skipped Test: ' . $e->getMessage(),
            $test
        );

        $this->currentTestPass = false;
    }

    /**
     * A testsuite started.
     *
     * @param TestInterface $suite
     */
    public function startTestSuite(TestInterface $suite)
    {
        $this->currentTestSuiteName = $suite->getName();
        $this->currentTestName      = '';

        $this->write(
            [
            'event' => 'suiteStart',
            'suite' => $this->currentTestSuiteName,
            'tests' => count($suite)
            ]
        );
    }

    /**
     * A testsuite ended.
     *
     * @param TestInterface $suite
     */
    public function endTestSuite(TestInterface $suite)
    {
        $this->currentTestSuiteName = '';
        $this->currentTestName      = '';
    }

    /**
     * A test started.
     *
     * @param TestInterface $test
     */
    public function startTest(TestInterface $test)
    {
        $this->currentTestName = PHPUnit_Util_Test::describe($test);
        $this->currentTestPass = true;

        $this->write(
            [
            'event' => 'testStart',
            'suite' => $this->currentTestSuiteName,
            'test'  => $this->currentTestName
            ]
        );
    }

    /**
     * A test ended.
     *
     * @param TestInterface $test
     * @param float                  $time
     */
    public function endTest(TestInterface $test, $time)
    {
        if ($this->currentTestPass) {
            $this->writeCase('pass', $time, [], '', $test);
        }
    }

    /**
     * @param string                          $status
     * @param float                           $time
     * @param array                           $trace
     * @param string                          $message
     * @param TestCase|null $test
     */
    protected function writeCase($status, $time, array $trace = [], $message = '', $test = null)
    {
        $output = '';
        // take care of TestSuite producing error (e.g. by running into exception) as TestSuite doesn't have hasOutput
        if ($test !== null && method_exists($test, 'hasOutput') && $test->hasOutput()) {
            $output = $test->getActualOutput();
        }
        $this->write(
            [
            'event'   => 'test',
            'suite'   => $this->currentTestSuiteName,
            'test'    => $this->currentTestName,
            'status'  => $status,
            'time'    => $time,
            'trace'   => $trace,
            'message' => PHPUnit_Util_String::convertToUtf8($message),
            'output'  => $output,
            ]
        );
    }

    /**
     * @param string $buffer
     */
    public function write($buffer)
    {
        array_walk_recursive($buffer, function (&$input) {
            if (is_string($input)) {
                $input = PHPUnit_Util_String::convertToUtf8($input);
            }
        });

        parent::write(json_encode($buffer, JSON_PRETTY_PRINT));
    }
}
