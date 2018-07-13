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
use PHPUnit\Exceptions\Exception as PHPUnit_Exceptions_Exception;
use PHPUnit\Exceptions\ExpectationFailedException;
use PHPUnit\Exceptions\Warning;
use PHPUnit\Framework\TestFailure;
use PHPUnit\Interfaces\TestInterface;
use PHPUnit\Interfaces\TestListener;
use PHPUnit\Interfaces\TestSuiteInterface;
use PHPUnit\Util\PrinterUtil;
use PHPUnit\Util\TestUtil;
use Symfony\Component\Yaml\Dumper;

use \Exception;

/**
 * A TestListener that generates a logfile of the
 * test execution using the Test Anything Protocol (TAP).
 *
 * @since Class available since Release 3.0.0
 */
class TAPUtil extends PrinterUtil implements TestListener {

  /**
   * @var int
   */
  protected int $testNumber = 0;

  /**
   * @var int
   */
  protected int $testSuiteLevel = 0;

  /**
   * @var bool
   */
  protected bool $testSuccessful = true;

  /**
   * Constructor.
   *
   * @param mixed $out
   *
   * @throws PHPUnit_Exceptions_Exception
   *
   * @since Method available since Release 3.3.4
   */
  public function __construct(mixed $out = null) {
    parent::__construct($out);
    $this->writeString("TAP version 13\n");
  }

  /**
   * An error occurred.
   *
   * @param PHPUnit_Framework_Test $test
   * @param Exception              $e
   * @param float                  $time
   */
  public function addError(TestInterface $test, Exception $e, float $time): void {
    $this->writeNotOk($test, 'Error');
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
      $this->writeNotOk($test, 'Warning');
    }

    /**
     * A failure occurred.
     *
     * @param PHPUnit_Framework_Test                 $test
     * @param AssertionFailedError $e
     * @param float                                  $time
     */
    public function addFailure(TestInterface $test, AssertionFailedError $e, float $time): void {
        $this->writeNotOk($test, 'Failure');

        $message = explode(
            "\n",
            TestFailure::exceptionToString($e)
        );

        $diagnostic = [
          'message'  => $message[0],
          'severity' => 'fail'
        ];

        if ($e instanceof ExpectationFailedException) {
            $cf = $e->getComparisonFailure();

            if ($cf !== null) {
                $diagnostic['data'] = [
                  'got'      => $cf->getActual(),
                  'expected' => $cf->getExpected()
                ];
            }
        }

        $yaml = new Dumper();

        $this->writeString(
            sprintf(
                "  ---\n%s  ...\n",
                $yaml->dump($diagnostic, 2, 2)
            )
        );
    }

    /**
     * Incomplete test.
     *
     * @param PHPUnit_Framework_Test $test
     * @param Exception              $e
     * @param float                  $time
     */
    public function addIncompleteTest(TestInterface $test, Exception $e, float $time): void {
      $this->writeNotOk($test, '', 'TODO Incomplete Test');
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

      $this->writeString(
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
     * @param PHPUnit_Framework_Test $test
     * @param Exception              $e
     * @param float                  $time
     *
     * @since Method available since Release 3.0.0
     */
    public function addSkippedTest(TestInterface $test, Exception $e, float $time): void {
        $this->writeString(
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
     * @param PHPUnit_Framework_TestSuite $suite
     */
    public function startTestSuite(TestSuiteInterface $suite): void {
      $this->testSuiteLevel++;
    }

    /**
     * A testsuite ended.
     *
     * @param PHPUnit_Framework_TestSuite $suite
     */
    public function endTestSuite(TestSuiteInterface $suite): void {

        $this->testSuiteLevel--;

        if ($this->testSuiteLevel == 0) {
            $this->writeString(sprintf("1..%d\n", $this->testNumber));
        }

    }

    /**
     * A test started.
     *
     * @param PHPUnit_Framework_Test $test
     */
    public function startTest(TestInterface $test): void {
      $this->testNumber++;
      $this->testSuccessful = true;
    }

    /**
     * A test ended.
     *
     * @param PHPUnit_Framework_Test $test
     * @param float                  $time
     */
    public function endTest(TestInterface $test, float $time): void
    {
        if ($this->testSuccessful === true) {
            $this->writeString(
                sprintf(
                    "ok %d - %s\n",
                    $this->testNumber,
                    TestUtil::describe($test)
                )
            );
        }

        $this->writeDiagnostics($test);
    }

    /**
     * @param PHPUnit_Framework_Test $test
     * @param string                 $prefix
     * @param string                 $directive
     */
    protected function writeNotOk(TestInterface $test, string $prefix = '', string $directive = ''): void
    {
        $this->writeString(
            sprintf(
                "not ok %d - %s%s%s\n",
                $this->testNumber,
                $prefix != '' ? $prefix . ': ' : '',
                TestUtil::describe($test),
                $directive != '' ? ' # ' . $directive : ''
            )
        );

        $this->testSuccessful = false;
    }

    /**
     * @param PHPUnit_Framework_Test $test
     */
    private function writeDiagnostics(TestInterface $test): void {

      if (!$test->hasOutput()) {
        return;
      }

      foreach (explode("\n", trim($test->getActualOutput())) as $line) {
        $this->writeString(
          sprintf(
            "# %s\n",
            $line
          )
        );
      }

    }

    public function writeHook(Map<string, mixed> $buffer): string {
      return '';
    }

}
