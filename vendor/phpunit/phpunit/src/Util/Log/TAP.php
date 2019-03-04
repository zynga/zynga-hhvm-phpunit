<?php
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

use \Exception;

/**
 * A TestListener that generates a logfile of the
 * test execution using the Test Anything Protocol (TAP).
 *
 * @since Class available since Release 3.0.0
 */
class PHPUnit_Util_Log_TAP extends PHPUnit_Util_Printer implements TestListenerInterface
{
    /**
     * @var int
     */
    protected $testNumber = 0;

    /**
     * @var int
     */
    protected $testSuiteLevel = 0;

    /**
     * @var bool
     */
    protected $testSuccessful = true;

    /**
     * Constructor.
     *
     * @param mixed $out
     *
     * @throws PHPUnit_Framework_Exception
     *
     * @since Method available since Release 3.3.4
     */
    public function __construct($out = null)
    {
        parent::__construct($out);
        $this->write("TAP version 13\n");
    }

    /**
     * An error occurred.
     *
     * @param TestInterface $test
     * @param Exception              $e
     * @param float                  $time
     */
    public function addError(TestInterface $test, Exception $e, $time)
    {
        $this->writeNotOk($test, 'Error');
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
        $this->writeNotOk($test, 'Warning');
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
        $this->writeNotOk($test, 'Failure');

        $message = explode(
            "\n",
            PHPUnit_Framework_TestFailure::exceptionToString($e)
        );

        $diagnostic = [
          'message'  => $message[0],
          'severity' => 'fail'
        ];

        if ($e instanceof PHPUnit_Framework_ExpectationFailedException) {
            $cf = $e->getComparisonFailure();

            if ($cf !== null) {
                $diagnostic['data'] = [
                  'got'      => $cf->getActual(),
                  'expected' => $cf->getExpected()
                ];
            }
        }

        $yaml = new Symfony\Component\Yaml\Dumper;

        $this->write(
            sprintf(
                "  ---\n%s  ...\n",
                $yaml->dump($diagnostic, 2, 2)
            )
        );
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
        $this->writeNotOk($test, '', 'TODO Incomplete Test');
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
        $this->write(
            sprintf(
                "ok %d - # RISKY%s\n",
                $this->testNumber,
                $e->getMessage() != '' ? ' ' . $e->getMessage() : ''
            )
        );

        $this->testSuccessful = false;
    }

    /**
     * Skipped test.
     *
     * @param TestInterface $test
     * @param Exception              $e
     * @param float                  $time
     *
     * @since Method available since Release 3.0.0
     */
    public function addSkippedTest(TestInterface $test, Exception $e, $time)
    {
        $this->write(
            sprintf(
                "ok %d - # SKIP%s\n",
                $this->testNumber,
                $e->getMessage() != '' ? ' ' . $e->getMessage() : ''
            )
        );

        $this->testSuccessful = false;
    }

    /**
     * A testsuite started.
     *
     * @param TestInterface $suite
     */
    public function startTestSuite(TestInterface $suite)
    {
        $this->testSuiteLevel++;
    }

    /**
     * A testsuite ended.
     *
     * @param TestInterface $suite
     */
    public function endTestSuite(TestInterface $suite)
    {
        $this->testSuiteLevel--;

        if ($this->testSuiteLevel == 0) {
            $this->write(sprintf("1..%d\n", $this->testNumber));
        }
    }

    /**
     * A test started.
     *
     * @param TestInterface $test
     */
    public function startTest(TestInterface $test)
    {
        $this->testNumber++;
        $this->testSuccessful = true;
    }

    /**
     * A test ended.
     *
     * @param TestInterface $test
     * @param float                  $time
     */
    public function endTest(TestInterface $test, $time)
    {
        if ($this->testSuccessful === true) {
            $this->write(
                sprintf(
                    "ok %d - %s\n",
                    $this->testNumber,
                    PHPUnit_Util_Test::describe($test)
                )
            );
        }

        $this->writeDiagnostics($test);
    }

    /**
     * @param TestInterface $test
     * @param string                 $prefix
     * @param string                 $directive
     */
    protected function writeNotOk(TestInterface $test, $prefix = '', $directive = '')
    {
        $this->write(
            sprintf(
                "not ok %d - %s%s%s\n",
                $this->testNumber,
                $prefix != '' ? $prefix . ': ' : '',
                PHPUnit_Util_Test::describe($test),
                $directive != '' ? ' # ' . $directive : ''
            )
        );

        $this->testSuccessful = false;
    }

    /**
     * @param TestInterface $test
     */
    private function writeDiagnostics(TestInterface $test)
    {
        if (!$test instanceof TestCase) {
            return;
        }

        if (!$test->hasOutput()) {
            return;
        }

        foreach (explode("\n", trim($test->getActualOutput())) as $line) {
            $this->write(
                sprintf(
                    "# %s\n",
                    $line
                )
            );
        }
    }
}
