<?hh // strict

namespace Zynga\PHPUnit\V2\Output;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use SebastianBergmann\Environment\Console;
use Zynga\Framework\Testing\TestCase\V2\Base as ZyngaTestCase;
use Zynga\PHPUnit\V2\Interfaces\TestInterface;
use Zynga\PHPUnit\V2\Interfaces\TestListenerInterface;
use Zynga\PHPUnit\V2\TestCase;
use Zynga\PHPUnit\V2\TestFailure;
use Zynga\PHPUnit\V2\TestResult;
use Zynga\PHPUnit\V2\TestSuite;
use Zynga\PHPUnit\V2\Output\Printer;

use Zynga\PHPUnit\V2\StackTrace\Stack as StackTrace;

use \Exception;

/**
 * Prints the result of a TextUI TestRunner run.
 *
 * @since Class available since Release 2.0.0
 */
class ResultPrinter extends Printer implements TestListenerInterface {
  const int EVENT_TEST_START = 0;
  const int EVENT_TEST_END = 1;
  const int EVENT_TESTSUITE_START = 2;
  const int EVENT_TESTSUITE_END = 3;

  const string COLOR_NEVER = 'never';
  const string COLOR_AUTO = 'auto';
  const string COLOR_ALWAYS = 'always';
  const string COLOR_DEFAULT = self::COLOR_NEVER;

  /**
   * @var array
   */
  private static Map<string, int>
    $ansiCodes = Map {
      'bold' => 1,
      'fg-black' => 30,
      'fg-red' => 31,
      'fg-green' => 32,
      'fg-yellow' => 33,
      'fg-blue' => 34,
      'fg-magenta' => 35,
      'fg-cyan' => 36,
      'fg-white' => 37,
      'bg-black' => 40,
      'bg-red' => 41,
      'bg-green' => 42,
      'bg-yellow' => 43,
      'bg-blue' => 44,
      'bg-magenta' => 45,
      'bg-cyan' => 46,
      'bg-white' => 47,
    };

  protected bool $lastTestFailed = false;
  protected int $numAssertions = 0;
  protected int $numTests = -1;
  protected int $numTestsRun = 0;
  protected bool $colors = false;
  protected bool $debug = false;
  protected bool $verbose = false;
  private int $numberOfColumns;
  private bool $reverse = false;
  private bool $defectListPrinted = false;
  private int $_slowTestMax;
  private int $_slowTestDisplayMax;
  private Map<string, int> $_slowTests;

  /**
   * Constructor.
   *
   * @param mixed      $out
   * @param bool       $verbose
   * @param string     $colors
   * @param bool       $debug
   * @param int|string $numberOfColumns
   * @param bool       $reverse
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since Method available since Release 3.0.0
   */
  public function __construct(
    mixed $out = null,
    bool $verbose = false,
    string $colors = self::COLOR_DEFAULT,
    bool $debug = false,
    int $numberOfColumns = 80,
    bool $reverse = false,
  ) {

    $console = new Console();
    $maxNumberOfColumns = $console->getNumberOfColumns();

    if ($numberOfColumns == 'max' || $numberOfColumns > $maxNumberOfColumns) {
      $numberOfColumns = $maxNumberOfColumns;
    }

    $this->numberOfColumns = $numberOfColumns;
    $this->verbose = $verbose;
    $this->debug = $debug;
    $this->reverse = $reverse;

    if ($colors === self::COLOR_AUTO && $console->hasColorSupport()) {
      $this->colors = true;
    } else {
      $this->colors = (self::COLOR_ALWAYS === $colors);
    }

    $this->_slowTestMax = 10; // 10ms by default.
    $this->_slowTestDisplayMax = 10; // only show 10 tests at max
    $this->_slowTests = Map {};

  }

  private function printSeperator(): void {
    $this->write("\n".str_repeat('-', 80)."\n");
  }

  /**
   * @param TestResult $result
   */
  public function printResult(TestResult $result): void {
    $this->printHeader();
    $this->printErrors($result);
    $this->printWarnings($result);
    $this->printFailures($result);

    if ($this->verbose) {
      $this->printRisky($result);
      $this->printIncompletes($result);
      $this->printSkipped($result);
    }

    $this->printFooter($result);
  }

  /**
   * @param array  $defects
   * @param string $type
   */
  protected function printDefects(
    Vector<TestFailure> $defects,
    string $type,
  ): void {
    $count = count($defects);

    if ($count == 0) {
      return;
    }

    if ($this->defectListPrinted) {
      $this->printSeperator();
      //$this->write("\n--\n\n");
    }

    $this->write(
      sprintf(
        "There %s %d %s%s:\n",
        ($count == 1) ? 'was' : 'were',
        $count,
        $type,
        ($count == 1) ? '' : 's',
      ),
    );

    $i = 1;

    if ($this->reverse) {
      $defects = array_reverse($defects);
    }

    foreach ($defects as $defect) {
      if ($i != 1) {
        $this->writeNewLine();
      }
      $this->printDefect($defect, $i++);
    }

    $this->defectListPrinted = true;
  }

  /**
   * @param TestFailure $defect
   * @param int                           $count
   */
  protected function printDefect(TestFailure $defect, int $count): void {
    $this->printDefectHeader($defect, $count);
    $this->printDefectTrace($defect);
  }

  /**
   * @param TestFailure $defect
   * @param int                           $count
   */
  protected function printDefectHeader(TestFailure $defect, int $count): void {
    $this->writeWithColor(
      "fg-red, bg-black",
      sprintf("\n%d) %s", $count, $defect->getTestName()),
    );
  }

  /**
   * @param TestFailure $defect
   */
  protected function printDefectTrace(TestFailure $defect): void {
    $e = $defect->thrownException();

    if ($e instanceof Exception) {

      $this->createCleanStackTrace($e);

      /*
       $this->write('----Legacy mode----');
       $this->write((string) $e);
       $previousException = $e->getPrevious();
       while ($previousException instanceof Exception) {
       $this->write("\nCaused by\n".$e);
       $previousException = $previousException->getPrevious();
       }
       */

    }

  }

  public function createCleanStackTrace(Exception $e): void {

    // --
    //
    // We build up a consolidated frame stack that includes all the
    //   information including inner exceptions as the trace is linear
    //   throughout.
    //
    // --

    $this->write("  Stacktrace:\n");

    $stack = new StackTrace();
    $stack->consumeException($e);

    $frameId = 0;
    $currentFile = '';

    $exceptPad = 3;
    $innerPad = 5;
    $tracePad = 7;

    foreach ($stack->getFrames() as $frame) {

      $file = $frame->getShortFileName();
      $line = $frame->getLine();
      $class = $frame->getClass();
      $function = $frame->getFunction();

      // Soon as we get to the phpunit test case, feel free to break out of the excetion trace.
      if ($class == 'Zynga\PHPUnit\V2\TestCase') {
        break;
      }

      $this->write(
        sprintf("[%d] %s#%d\n", $frameId, $file, $line),
        $exceptPad,
      );

      $exception = $frame->getException();

      if ($exception instanceof Exception) {
        $this->write("Exception trace:\n", $innerPad);

        $exceptionId = 1;

        // write out the parent exception first.
        $this->write(
          sprintf(
            "[E%03d] message=%s\n",
            $exceptionId,
            $this->removeControlCharacters($exception->getMessage()),
          ),
          $innerPad,
        );

        $this->write(sprintf("class=%s\n", get_class($exception)), $tracePad);

        $this->write(
          sprintf(
            "file=%s\n",
            $frame->shortenFileName($exception->getFile()),
          ),
          $tracePad,
        );

        $this->write(sprintf("line=%d\n", $exception->getLine()), $tracePad);

        // loop across all the previous allowing them to stab in their exception moments.
        $previousException = $e->getPrevious();

        while ($previousException instanceof Exception) {
          $exceptionId++;

          $this->write(
            sprintf(
              "[E%03d] message=%s\n",
              $exceptionId,
              $this->removeControlCharacters(
                $previousException->getMessage(),
              ),
            ),
            $innerPad,
          );

          $this->write(
            sprintf("class=%s\n", get_class($previousException)),
            $tracePad,
          );

          $this->write(
            sprintf(
              "file=%s\n",
              $frame->shortenFileName($previousException->getFile()),
            ),
            $tracePad,
          );

          $this->write(
            sprintf("line=%d\n", $previousException->getLine()),
            $tracePad,
          );

          $previousException = $previousException->getPrevious();
        }

      }

      $this->write(sprintf("call=%s::%s\n", $class, $function), $tracePad);

      $frameId++;

    }

  }

  public function removeControlCharacters(string $input): string {
    return preg_replace('/[[:cntrl:]]/', '[xTCCx]', $input);
  }

  /**
   * Convert PHPUnit's reported test time (microseconds) to milliseconds.
   *
   * @param float $time
   * @return int
   */
  protected function toMilliseconds(float $time): int {
    return (int) round($time * 1000);
  }

  /**
   * @param TestResult $result
   */
  protected function printErrors(TestResult $result): void {
    $this->printDefects($result->errors(), 'error');
  }

  /**
   * @param TestResult $result
   */
  protected function printFailures(TestResult $result): void {
    $this->printDefects($result->failures(), 'failure');
  }

  /**
   * @param TestResult $result
   */
  protected function printWarnings(TestResult $result): void {
    $this->printDefects($result->warnings(), 'warning');
  }

  /**
   * @param TestResult $result
   */
  protected function printIncompletes(TestResult $result): void {
    $this->printDefects($result->notImplemented(), 'incomplete test');
  }

  /**
   * @param TestResult $result
   *
   * @since Method available since Release 4.0.0
   */
  protected function printRisky(TestResult $result): void {
    $this->printDefects($result->risky(), 'risky test');
  }

  /**
   * @param TestResult $result
   *
   * @since Method available since Release 3.0.0
   */
  protected function printSkipped(TestResult $result): void {
    $this->printDefects($result->skipped(), 'skipped test');
    $this->printSeperator();
    //$this->write("\n--\n");
  }

  protected function printHeader(): void {

    // $perfData = sprintf(
    //   'Time: %s, Memory: %4.2fMB',
    //   'TODO::NEED_TO_USE_PERF_TIMERS',
    //   memory_get_peak_usage(true) / 1048576,
    // );
    // $this->write("\n\n$perfData\n\n");

    $this->printSeperator();
  }

  /**
   * @param TestResult $result
   */
  protected function printFooter(TestResult $result): void {

    if (count($result) === 0) {
      $this->writeWithColor('fg-black, bg-yellow', 'No tests executed!');
      return;
    }

    if ($this->_slowTests->count() > 0) {

      $perfColor = 'bg-black, fg-red';

      $this->writeWithColor(
        $perfColor,
        sprintf(
          "\nYou should really fix these slow tests (>%dms)",
          $this->_slowTestMax,
        ),
      );

      $i = 0;

      //$sortedSlowTests = $this->_slowTests->toArray();

      arsort($this->_slowTests);

      foreach ($this->_slowTests as $slowTest => $elapsed) {

        $i++;

        if ($i > $this->_slowTestDisplayMax) {
          $this->writeWithColor(
            $perfColor,
            sprintf('Omitted %d tests...', $this->_slowTests->count() - $i),
          );
          break;
        }

        $this->writeWithColor(
          $perfColor,
          sprintf(' %4d. %dms to run %s', $i, $elapsed, $slowTest),
        );

      }

      $this->printSeperator();

      $this->writeNewLine();

    }

    $color = '';

    if ($result->wasSuccessful() &&
        $result->allHarmless() &&
        $result->allCompletelyImplemented() &&
        $result->noneSkipped()) {
      $this->writeWithColor(
        'fg-black, bg-green',
        sprintf(
          'OK (%d test%s, %d assertion%s)',
          $result->count(),
          (count($result) == 1) ? '' : 's',
          $this->numAssertions,
          ($this->numAssertions == 1) ? '' : 's',
        ),
      );
    } else {
      if ($result->wasSuccessful()) {
        $color = 'fg-black, bg-yellow';

        if ($this->verbose) {
          $this->write("\n");
        }

        $this->writeWithColor(
          $color,
          'OK, but incomplete, skipped, or risky tests!',
        );
      } else {
        $this->write("\n");

        if ($result->errorCount()) {
          $color = 'fg-white, bg-red';

          $this->writeWithColor($color, 'ERRORS!');
        } else if ($result->failureCount()) {
          $color = 'fg-white, bg-red';

          $this->writeWithColor($color, 'FAILURES!');

        } else if ($result->warningCount()) {
          $color = 'fg-black, bg-yellow';

          $this->writeWithColor($color, 'WARNINGS!');
        }
      }

      $this->writeCountString($this->numTests, 'Tests', $color, true);

      $this->writeCountString(
        $this->numAssertions,
        'Assertions',
        $color,
        true,
      );
      $this->writeCountString($result->errorCount(), 'Errors', $color);
      $this->writeCountString($result->failureCount(), 'Failures', $color);
      $this->writeCountString($result->warningCount(), 'Warnings', $color);
      $this->writeCountString($result->skippedCount(), 'Skipped', $color);
      $this->writeCountString(
        $result->notImplementedCount(),
        'Incomplete',
        $color,
      );
      $this->writeCountString($result->riskyCount(), 'Risky', $color);
      $this->writeWithColor($color, '.', true);
    }

  }

  /**
   */
  public function printWaitPrompt(): void {
    $this->write("\n<RETURN> to continue\n");
  }

  /**
   * An error occurred.
   *
   * @param TestInterface $test
   * @param Exception              $e
   * @param float                  $time
   */
  public function addError(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {
    $this->numTestsRun++;

    if ($this->debug) {
      $this->printSingleTestDebugLine($test, 'ERROR', $time, 'fg-red, bold');
    } else {
      $this->writeProgressWithColor('fg-red, bold', 'E');
    }

    $this->lastTestFailed = true;
  }

  /**
   * A failure occurred.
   *
   * @param TestInterface                 $test
   * @param Exception $e
   * @param float                                  $time
   */
  public function addFailure(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {
    $this->numTestsRun++;

    if ($this->debug) {
      $this->printSingleTestDebugLine(
        $test,
        'FAILED',
        $time,
        'bg-red, fg-white',
      );
    } else {
      $this->writeProgressWithColor('bg-red, fg-white', 'F');
    }

    $this->lastTestFailed = true;
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
  public function addWarning(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {
    $this->numTestsRun++;

    if ($this->debug) {
      $this->printSingleTestDebugLine(
        $test,
        'WARNING',
        $time,
        'fg-yellow, bold',
      );
    } else {
      $this->writeProgressWithColor('fg-yellow, bold', 'W');
    }

    $this->lastTestFailed = true;
  }

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
  ): void {

    //$this->numTestsRun++;

    if ($this->debug) {
      $this->printSingleTestDebugLine(
        $test,
        'INCOMPLETE',
        $time,
        'fg-yellow, bold',
      );
    } else {
      $this->writeProgressWithColor('fg-yellow, bold', 'I');
    }

    $this->lastTestFailed = true;
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
  public function addRiskyTest(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {

    $this->numTestsRun++;

    if ($this->debug) {
      $this->printSingleTestDebugLine(
        $test,
        'RISKY',
        $time,
        'fg-yellow, bold',
      );
    } else {
      $this->writeProgressWithColor('fg-yellow, bold', 'R');
    }

    $this->lastTestFailed = true;
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
  public function addSkippedTest(
    TestInterface $test,
    Exception $e,
    float $time,
  ): void {

    $this->numTestsRun++;

    if ($this->debug) {
      $this->printSingleTestDebugLine(
        $test,
        'SKIPPED',
        $time,
        'fg-cyan, bold',
      );
    } else {
      $this->writeProgressWithColor('fg-cyan, bold', 'S');
    }

    $this->lastTestFailed = true;
  }

  /**
   * A testsuite started.
   *
   * @param TestSuite $suite
   *
   * @since Method available since Release 2.2.0
   */
  public function startTestSuite(TestInterface $suite): void {
    if ($this->numTests == -1) {
      $this->numTests = $suite->count();
    }
  }

  /**
   * A testsuite ended.
   *
   * @param TestSuite $suite
   *
   * @since Method available since Release 2.2.0
   */
  public function endTestSuite(TestInterface $suite): void {}

  /**
   * A test started.
   *
   * @param TestInterface $test
   */
  public function startTest(TestInterface $test): void {

    $testName = $test->getName();

    $this->writeWithColor(
      'bg-black, fg-green',
      date('r').sprintf(" - '%s'", get_class($test).'::'.$testName),
    );

    $this->printSingleTestDebugLine($test, 'Started', 0.0);

  }

  private function printSingleTestDebugLine(
    TestInterface $test,
    string $message,
    float $elapsed,
    string $color = 'bg-black, fg-green',
  ): void {

    if ($this->debug !== true) {
      return;
    }

    $testName = $test->getName();

    $elapsedMS = $this->toMilliseconds($elapsed);

    if ($elapsedMS > $this->_slowTestMax) {
      $message .= ', but elapsed > '.$this->_slowTestMax.'ms';
      $color = 'bg-black, fg-red';
      $this->_slowTests->set($testName, $elapsedMS);
    }

    $this->writeWithColor(
      $color,
      date('r').
      sprintf(
        " - %30s - [%4dms elapsed] - Overall state: %6d / %6d (%3s%%)",
        $message,
        $elapsedMS,
        $this->numTestsRun,
        $this->numTests,
        floor(($this->numTestsRun / $this->numTests) * 100),
      ),
    );

  }

  /**
   * A test ended.
   *
   * @param TestInterface $test
   * @param float                  $time
   */
  public function endTest(TestInterface $test, float $time): void {

    $this->numTestsRun++;

    $testName = $test->getName();

    if (!$this->lastTestFailed) {
      if ($this->debug) {
        $this->printSingleTestDebugLine($test, 'OK', $time);
      } else {
        $this->writeProgress('.');
      }
    }

    if ($test instanceof TestCase) {
      $this->numAssertions += $test->getNumAssertions();
    } else if ($test instanceof ZyngaTestCase) {
      $this->numAssertions += $test->getNumAssertions();
    }

    $this->lastTestFailed = false;

    if ($test instanceof TestCase) {
      if (!$test->hasExpectationOnOutput()) {
        $this->write($test->getActualOutput());
      }
    }

  }

  /**
   * @param string $progress
   */
  protected function writeProgress(string $progress): void {

    $this->write($progress);
    // $this->column++;

    // if ($this->column == $this->maxColumn ||
    //     $this->numTestsRun == $this->numTests) {
    //   if ($this->numTestsRun == $this->numTests) {
    //     $this->write(str_repeat(' ', $this->maxColumn - $this->column));
    //   }

    //   // JEO: hardcoded test width to 6x6
    //   $this->write(
    //     sprintf(
    //       ' %6d / %6d (%3s%%)',
    //       $this->numTestsRun,
    //       $this->numTests,
    //       floor(($this->numTestsRun / $this->numTests) * 100),
    //     ),
    //   );

    //   if ($this->column == $this->maxColumn) {
    //     $this->writeNewLine();
    //   }
    // }
  }

  protected function writeNewLine(): void {
    // $this->column = 0;
    $this->write("\n");
  }

  /**
   * Formats a buffer with a specified ANSI color sequence if colors are
   * enabled.
   *
   * @param string $color
   * @param string $buffer
   *
   * @return string
   *
   * @since Method available since Release 4.0.0
   */
  protected function formatWithColor(string $color, string $buffer): string {
    if (!$this->colors) {
      return $buffer;
    }

    //ORIG: $codes   = array_map('trim', explode(',', $color));
    $colors = explode(',', $color);
    $codes = array();
    foreach ($colors as $c) {
      $codes[] = trim($c);
    }

    $lines = explode("\n", $buffer);

    // ORIG: $padding = max(array_map('strlen', $lines));
    $lineData = array();
    foreach ($lines as $line) {
      $lineData[] = strlen($line);
    }
    $padding = max($lineData);

    $styles = [];

    foreach ($codes as $code) {
      $styles[] = self::$ansiCodes[$code];
    }

    $style = sprintf("\x1b[%sm", implode(';', $styles));

    $styledLines = [];

    foreach ($lines as $line) {
      $styledLines[] = $style.str_pad($line, $padding)."\x1b[0m";
    }

    return implode("\n", $styledLines);
  }

  /**
   * Writes a buffer out with a color sequence if colors are enabled.
   *
   * @param string $color
   * @param string $buffer
   * @param bool   $lf
   *
   * @since Method available since Release 4.0.0
   */
  protected function writeWithColor(
    string $color,
    string $buffer,
    bool $lf = true,
  ): void {
    $this->write($this->formatWithColor($color, $buffer));

    if ($lf) {
      $this->write("\n");
    }
  }

  /**
   * Writes progress with a color sequence if colors are enabled.
   *
   * @param string $color
   * @param string $buffer
   *
   * @since Method available since Release 4.0.0
   */
  protected function writeProgressWithColor(
    string $color,
    string $buffer,
  ): void {
    $buffer = $this->formatWithColor($color, $buffer);
    $this->writeProgress($buffer);
  }

  /**
   * @param int    $count
   * @param string $name
   * @param string $color
   * @param bool   $always
   *
   * @since Method available since Release 4.6.5
   */
  private function writeCountString(
    int $count,
    string $name,
    string $color,
    bool $always = false,
  ): void {
    static $first = true;

    if ($always || $count > 0) {
      $this->writeWithColor(
        $color,
        sprintf('%s%s: %d', !$first ? ', ' : '', $name, $count),
        false,
      );

      $first = false;
    }
  }
}
