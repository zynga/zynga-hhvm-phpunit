<?hh

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

/**
 * Prints the result of a TextUI TestRunner run.
 *
 * @since Class available since Release 2.0.0
 */
class PHPUnit_TextUI_ResultPrinter extends PHPUnit_Util_Printer
  implements TestListenerInterface {
  const EVENT_TEST_START = 0;
  const EVENT_TEST_END = 1;
  const EVENT_TESTSUITE_START = 2;
  const EVENT_TESTSUITE_END = 3;

  const COLOR_NEVER = 'never';
  const COLOR_AUTO = 'auto';
  const COLOR_ALWAYS = 'always';
  const COLOR_DEFAULT = self::COLOR_NEVER;

  /**
   * @var array
   */
  private static
    $ansiCodes = [
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
    ];

  /**
   * @var int
   */
  protected $column = 0;

  /**
   * @var int
   */
  protected $maxColumn;

  /**
   * @var bool
   */
  protected $lastTestFailed = false;

  /**
   * @var int
   */
  protected $numAssertions = 0;

  /**
   * @var int
   */
  protected $numTests = -1;

  /**
   * @var int
   */
  protected $numTestsRun = 0;

  /**
   * @var int
   */
  protected $numTestsWidth;

  /**
   * @var bool
   */
  protected $colors = false;

  /**
   * @var bool
   */
  protected $debug = false;

  /**
   * @var bool
   */
  protected $verbose = false;

  /**
   * @var int
   */
  private $numberOfColumns;

  /**
   * @var bool
   */
  private $reverse = false;

  /**
   * @var bool
   */
  private $defectListPrinted = false;

  private int $_slowTestMax;
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
    $out = null,
    $verbose = false,
    $colors = self::COLOR_DEFAULT,
    $debug = false,
    $numberOfColumns = 80,
    $reverse = false,
  ) {
    parent::__construct($out);

    if (!is_bool($verbose)) {
      throw PHPUnit_Util_InvalidArgumentHelper::factory(2, 'boolean');
    }

    $availableColors =
      [self::COLOR_NEVER, self::COLOR_AUTO, self::COLOR_ALWAYS];

    if (!in_array($colors, $availableColors)) {
      throw PHPUnit_Util_InvalidArgumentHelper::factory(
        3,
        vsprintf('value from "%s", "%s" or "%s"', $availableColors),
      );
    }

    if (!is_bool($debug)) {
      throw PHPUnit_Util_InvalidArgumentHelper::factory(4, 'boolean');
    }

    if (!is_int($numberOfColumns) && $numberOfColumns != 'max') {
      throw PHPUnit_Util_InvalidArgumentHelper::factory(
        5,
        'integer or "max"',
      );
    }

    if (!is_bool($reverse)) {
      throw PHPUnit_Util_InvalidArgumentHelper::factory(6, 'boolean');
    }

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
    $this->_slowTests = Map {};

  }

  /**
   * @param TestResult $result
   */
  public function printResult(TestResult $result) {
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
  protected function printDefects(Vector<TestFailure> $defects, $type) {
    $count = count($defects);

    if ($count == 0) {
      return;
    }

    if ($this->defectListPrinted) {
      $this->write("\n--\n\n");
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
      $this->printDefect($defect, $i++);
    }

    $this->defectListPrinted = true;
  }

  /**
   * @param TestFailure $defect
   * @param int                           $count
   */
  protected function printDefect(TestFailure $defect, $count) {
    $this->printDefectHeader($defect, $count);
    $this->printDefectTrace($defect);
  }

  /**
   * @param TestFailure $defect
   * @param int                           $count
   */
  protected function printDefectHeader(TestFailure $defect, $count) {
    $this->write(sprintf("\n%d) %s\n", $count, $defect->getTestName()));
  }

  /**
   * @param TestFailure $defect
   */
  protected function printDefectTrace(TestFailure $defect) {
    $e = $defect->thrownException();

    if ($e instanceof Exception) {

      $this->write((string) $e);

      $previousException = $e->getPrevious();

      while ($previousException instanceof Exception) {
        $this->write("\nCaused by\n".$e);
        $previousException = $previousException->getPrevious();
      }

    }

  }

  /**
   * Convert PHPUnit's reported test time (microseconds) to milliseconds.
   *
   * @param float $time
   * @return int
   */
  protected function toMilliseconds($time) {
    return (int) round($time * 1000);
  }

  /**
   * @param TestResult $result
   */
  protected function printErrors(TestResult $result) {
    $this->printDefects($result->errors(), 'error');
  }

  /**
   * @param TestResult $result
   */
  protected function printFailures(TestResult $result) {
    $this->printDefects($result->failures(), 'failure');
  }

  /**
   * @param TestResult $result
   */
  protected function printWarnings(TestResult $result) {
    $this->printDefects($result->warnings(), 'warning');
  }

  /**
   * @param TestResult $result
   */
  protected function printIncompletes(TestResult $result) {
    $this->printDefects($result->notImplemented(), 'incomplete test');
  }

  /**
   * @param TestResult $result
   *
   * @since Method available since Release 4.0.0
   */
  protected function printRisky(TestResult $result) {
    $this->printDefects($result->risky(), 'risky test');
  }

  /**
   * @param TestResult $result
   *
   * @since Method available since Release 3.0.0
   */
  protected function printSkipped(TestResult $result) {
    $this->printDefects($result->skipped(), 'skipped test');
  }

  protected function printHeader() {
    $this->write("\n\n".PHP_Timer::resourceUsage()."\n\n");
  }

  /**
   * @param TestResult $result
   */
  protected function printFooter(TestResult $result) {

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

        $this->writeWithColor(
          $perfColor,
          sprintf(' %4d. %dms to run %s', $i, $elapsed, $slowTest),
        );

      }

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
          count($result),
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

      $this->writeCountString(count($result), 'Tests', $color, true);
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
  public function printWaitPrompt() {
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

    $this->numTestsRun++;

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
      $this->numTestsWidth = strlen((string) $this->numTests);
      $this->maxColumn =
        $this->numberOfColumns -
        strlen('  /  (XXX%)') -
        (2 * $this->numTestsWidth);
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

    $testName = PHPUnit_Util_Test::describe($test);

    $this->writeWithColor(
      'bg-black, fg-green',
      date('r').sprintf(" - '%s'", $testName),
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

    $testName = PHPUnit_Util_Test::describe($test);

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

    $testName = PHPUnit_Util_Test::describe($test);

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
    } else if ($test instanceof PHPUnit_Extensions_PhptTestCase) {
      $this->numAssertions++;
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
  protected function writeProgress($progress) {
    $this->write($progress);
    $this->column++;

    if ($this->column == $this->maxColumn ||
        $this->numTestsRun == $this->numTests) {
      if ($this->numTestsRun == $this->numTests) {
        $this->write(str_repeat(' ', $this->maxColumn - $this->column));
      }

      // JEO: hardcoded test width to 6x6
      $this->write(
        sprintf(
          ' %6d / %6d (%3s%%)',
          $this->numTestsRun,
          $this->numTests,
          floor(($this->numTestsRun / $this->numTests) * 100),
        ),
      );

      if ($this->column == $this->maxColumn) {
        $this->writeNewLine();
      }
    }
  }

  protected function writeNewLine() {
    $this->column = 0;
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
  protected function formatWithColor($color, $buffer) {
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
  protected function writeWithColor($color, $buffer, $lf = true) {
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
  protected function writeProgressWithColor($color, $buffer) {
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
  private function writeCountString($count, $name, $color, $always = false) {
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
