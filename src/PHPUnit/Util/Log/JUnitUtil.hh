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
use PHPUnit\Interfaces\SelfDescribingInterface;
use PHPUnit\Interfaces\TestInterface;
use PHPUnit\Interfaces\TestListener;
use PHPUnit\Interfaces\TestSuiteInterface;
use PHPUnit\Util\FilterUtil;
use PHPUnit\Util\PrinterUtil;
use PHPUnit\Util\XMLUtil;

use \DOMDocument;
use \DOMElement;
use \Exception;
use \ReflectionClass;
use \ReflectionException;

/**
 * A TestListener that generates a logfile of the test execution in XML markup.
 *
 * The XML markup used is the same as the one that is used by the JUnit Ant task.
 *
 * @since Class available since Release 2.1.0
 */
class JUnitUtil extends PrinterUtil implements TestListener {
    /**
     * @var DOMDocument
     */
    protected DOMDocument $document;

    /**
     * @var DOMElement
     */
    protected DOMElement $root;

    /**
     * @var bool
     */
    protected bool $logIncompleteSkipped = false;

    /**
     * @var bool
     */
    protected bool $writeDocument = true;

    /**
     * @var DOMElement[]
     */
    protected Vector<DOMElement> $testSuites = Vector {};

    /**
     * @var int[]
     */
    protected Vector<int> $testSuiteTests = Vector {0};

    /**
     * @var int[]
     */
    protected Vector<int> $testSuiteAssertions = Vector {0};

    /**
     * @var int[]
     */
    protected Vector <int> $testSuiteErrors = Vector {0};

    /**
     * @var int[]
     */
    protected Vector<int> $testSuiteFailures = Vector {0};

    /**
     * @var int[]
     */
    protected Map <int, float> $testSuiteTimes = Map { 0 => 0.0};

    /**
     * @var int
     */
    protected int $testSuiteLevel = 0;

    /**
     * @var DOMElement
     */
    protected ?DOMElement $currentTestCase = null;

    /**
     * @var bool
     */
    protected bool $attachCurrentTestCase = true;

    /**
     * Constructor.
     *
     * @param mixed $out
     * @param bool  $logIncompleteSkipped
     */
    public function __construct(mixed $out = null, bool $logIncompleteSkipped = false) {

      $this->document               = new DOMDocument('1.0', 'UTF-8');
      $this->document->formatOutput = true;

      $this->root = $this->document->createElement('testsuites');
      $this->document->appendChild($this->root);

      parent::__construct($out);

      $this->logIncompleteSkipped = $logIncompleteSkipped;

    }

    /**
     * Flush buffer and close output.
     */
    public function flush(): void
    {
        if ($this->writeDocument === true) {
            $this->writeString($this->getXML());
        }

        parent::flush();
    }

    /**
     * An error occurred.
     *
     * @param TestInterface $test
     * @param Exception              $e
     * @param float                  $time
     */
    public function addError(TestInterface $test, Exception $e, float $time): void {
      $this->doAddFault($test, $e, $time, 'error');
      $this->testSuiteErrors[$this->testSuiteLevel]++;
    }

    /**
     * A warning occurred.
     *
     * @param TestInterface    $test
     * @param Warning $e
     * @param float                     $time
     *
     * @since Method available since Release 5.1.0
     */
    public function addWarning(TestInterface $test, Warning $e, float $time): void
    {
        $this->doAddFault($test, $e, $time, 'warning');
        $this->testSuiteFailures[$this->testSuiteLevel]++;
    }

    /**
     * A failure occurred.
     *
     * @param TestInterface                 $test
     * @param AssertionFailedError $e
     * @param float                                  $time
     */
    public function addFailure(TestInterface $test, AssertionFailedError $e, float $time): void {
        $this->doAddFault($test, $e, $time, 'failure');
        $this->testSuiteFailures[$this->testSuiteLevel]++;
    }

    /**
     * Incomplete test.
     *
     * @param TestInterface $test
     * @param Exception              $e
     * @param float                  $time
     */
    public function addIncompleteTest(TestInterface $test, Exception $e, float $time): void {

      if ($this->logIncompleteSkipped && $this->currentTestCase !== null) {
          $error = $this->document->createElement(
            'error',
            XMLUtil::prepareString(
            "Incomplete Test\n" .
            FilterUtil::getFilteredStacktrace($e)
            )
          );

          $error->setAttribute('type', get_class($e));

          if ( $this->currentTestCase instanceof DOMElement ) {
            $this->currentTestCase->appendChild($error);
          }

          $this->testSuiteErrors[$this->testSuiteLevel]++;

        } else {
          $this->attachCurrentTestCase = false;
        }

    }

    /**
     * Risky test.
     *
     * @param TestInterface $test
     * @param Exception              $e
     * @param float                  $time
     *
     * @since  Method available since Release 4.0.0
     */
    public function addRiskyTest(TestInterface $test, Exception $e, float $time): void {

        if ($this->logIncompleteSkipped && $this->currentTestCase !== null) {
            $error = $this->document->createElement(
                'error',
                XMLUtil::prepareString(
                    "Risky Test\n" .
                    FilterUtil::getFilteredStacktrace($e)
                )
            );

            $error->setAttribute('type', get_class($e));

            if ( $this->currentTestCase instanceof DOMElement ) {
              $this->currentTestCase->appendChild($error);
            }

            $this->testSuiteErrors[$this->testSuiteLevel]++;

        } else {
            $this->attachCurrentTestCase = false;
        }
    }

    /**
     * Skipped test.
     *
     * @param TestInterface $test
     * @param Exception              $e
     * @param float                  $time
     *
     * @since  Method available since Release 3.0.0
     */
    public function addSkippedTest(TestInterface $test, Exception $e, float $time): void {

      if ($this->logIncompleteSkipped && $this->currentTestCase !== null) {
        $error = $this->document->createElement(
          'error',
          XMLUtil::prepareString(
          "Skipped Test\n" .
          FilterUtil::getFilteredStacktrace($e)
          )
        );

        $error->setAttribute('type', get_class($e));

        if ( $this->currentTestCase instanceof DOMElement ) {
          $this->currentTestCase->appendChild($error);
        }

        $this->testSuiteErrors[$this->testSuiteLevel]++;

      } else {

        $this->attachCurrentTestCase = false;

      }

    }

    /**
     * A testsuite started.
     *
     * @param TestSuiteInterface $suite
     *
     * @since  Method available since Release 2.2.0
     */
    public function startTestSuite(TestSuiteInterface $suite): void {

      $testSuite = $this->document->createElement('testsuite');
      $testSuite->setAttribute('name', $suite->getName());

      if (class_exists($suite->getName(), false)) {

        try {

          $class = new ReflectionClass($suite->getName());
          $testSuite->setAttribute('file', $class->getFileName());
        } catch (ReflectionException $e) {
        }

      }

      if ($this->testSuiteLevel > 0) {
        $this->testSuites[$this->testSuiteLevel]->appendChild($testSuite);
      } else {
        $this->root->appendChild($testSuite);
      }

      $this->testSuiteLevel++;
      $this->testSuites[$this->testSuiteLevel]          = $testSuite;
      $this->testSuiteTests[$this->testSuiteLevel]      = 0;
      $this->testSuiteAssertions[$this->testSuiteLevel] = 0;
      $this->testSuiteErrors[$this->testSuiteLevel]     = 0;
      $this->testSuiteFailures[$this->testSuiteLevel]   = 0;
      $this->testSuiteTimes[$this->testSuiteLevel]      = 0.0;

    }

    /**
     * A testsuite ended.
     *
     * @param TestSuiteInterface $suite
     *
     * @since  Method available since Release 2.2.0
     */
    public function endTestSuite(TestSuiteInterface $suite): void {

      $this->testSuites[$this->testSuiteLevel]->setAttribute(
        'tests',
        $this->testSuiteTests[$this->testSuiteLevel]
      );

      $this->testSuites[$this->testSuiteLevel]->setAttribute(
        'assertions',
        $this->testSuiteAssertions[$this->testSuiteLevel]
      );

      $this->testSuites[$this->testSuiteLevel]->setAttribute(
        'failures',
        $this->testSuiteFailures[$this->testSuiteLevel]
      );

      $this->testSuites[$this->testSuiteLevel]->setAttribute(
        'errors',
        $this->testSuiteErrors[$this->testSuiteLevel]
      );

      $this->testSuites[$this->testSuiteLevel]->setAttribute(
        'time',
        sprintf('%F', $this->testSuiteTimes[$this->testSuiteLevel])
      );

      if ($this->testSuiteLevel > 1) {
        $this->testSuiteTests[$this->testSuiteLevel - 1]      += $this->testSuiteTests[$this->testSuiteLevel];
        $this->testSuiteAssertions[$this->testSuiteLevel - 1] += $this->testSuiteAssertions[$this->testSuiteLevel];
        $this->testSuiteErrors[$this->testSuiteLevel - 1]     += $this->testSuiteErrors[$this->testSuiteLevel];
        $this->testSuiteFailures[$this->testSuiteLevel - 1]   += $this->testSuiteFailures[$this->testSuiteLevel];
        $this->testSuiteTimes[$this->testSuiteLevel - 1]      += $this->testSuiteTimes[$this->testSuiteLevel];
      }

      $this->testSuiteLevel--;

    }

    /**
     * A test started.
     *
     * @param TestInterface $test
     */
    public function startTest(TestInterface $test): void {

        $testCase = $this->document->createElement('testcase');
        $testCase->setAttribute('name', $test->getName());

        if ($test instanceof TestInterface) {
            $class      = new ReflectionClass($test);
            $methodName = $test->getName();

            if ($class->hasMethod($methodName)) {
                $method = $class->getMethod($test->getName());

                $testCase->setAttribute('class', $class->getName());
                $testCase->setAttribute('file', $class->getFileName());
                $testCase->setAttribute('line', $method->getStartLine());
            }
        }

        $this->currentTestCase = $testCase;
    }

    /**
     * A test ended.
     *
     * @param TestInterface $test
     * @param float                  $time
     */
    public function endTest(TestInterface $test, float $time): void {

      if ($this->attachCurrentTestCase) {

        $numAssertions = $test->getNumAssertions();
        $this->testSuiteAssertions[$this->testSuiteLevel] += $numAssertions;
        $currentTestCase = $this->currentTestCase;

        if ( $currentTestCase instanceof DOMElement ) {

          $currentTestCase->setAttribute(
            'assertions',
            $numAssertions
          );

          $currentTestCase->setAttribute(
            'time',
            sprintf('%F', $time)
          );

          $this->testSuites[$this->testSuiteLevel]->appendChild(
            $currentTestCase
          );

        }



        $this->testSuiteTests[$this->testSuiteLevel]++;
        $this->testSuiteTimes[$this->testSuiteLevel] += $time;

        if (method_exists($test, 'hasOutput') && $test->hasOutput()) {
            $systemOut = $this->document->createElement('system-out');
            $systemOut->appendChild(
              $this->document->createTextNode($test->getActualOutput())
            );

            if ( $this->currentTestCase instanceof DOMElement ) {
              $this->currentTestCase->appendChild($systemOut);
            }

        }
      }

      $this->attachCurrentTestCase = true;
      $this->currentTestCase       = null;

    }

    /**
     * Returns the XML as a string.
     *
     * @return string
     *
     * @since  Method available since Release 2.2.0
     */
    public function getXML(): string {
      return $this->document->saveXML();
    }

    /**
     * Enables or disables the writing of the document
     * in flush().
     *
     * This is a "hack" needed for the integration of
     * PHPUnit with Phing.
     *
     * @param bool $flag
     *
     * @since  Method available since Release 2.2.0
     */
    public function setWriteDocument(bool $flag): void
    {
        if (is_bool($flag)) {
            $this->writeDocument = $flag;
        }
    }

    /**
     * Method which generalizes addError() and addFailure()
     *
     * @param TestInterface $test
     * @param Exception              $e
     * @param float                  $time
     * @param string                 $type
     */
    private function doAddFault(TestInterface $test, Exception $e, float $time, string $type): void {
        if ($this->currentTestCase === null) {
            return;
        }

        if ($test instanceof SelfDescribingInterface) {
            $buffer = $test->toString() . "\n";
        } else {
            $buffer = '';
        }

        $buffer .= TestFailure::exceptionToString($e) .
                   "\n" .
                   FilterUtil::getFilteredStacktrace($e);

        $fault = $this->document->createElement(
            $type,
            XMLUtil::prepareString($buffer)
        );

        $fault->setAttribute('type', get_class($e));

        if ( $this->currentTestCase instanceof DOMElement ) {
          $this->currentTestCase->appendChild($fault);
        }
    }

    public function writeHook(Map<string, mixed> $buffer): string {
      return '';
    }
    
}
