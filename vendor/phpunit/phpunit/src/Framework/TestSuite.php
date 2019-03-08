<?hh // decl

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use SebastianBergmann\PHPUnit\Exceptions\TestError\IncompleteException;
use SebastianBergmann\PHPUnit\Exceptions\TestError\SkippedException;
use
  SebastianBergmann\PHPUnit\Exceptions\TestSuiteError\SkippedException as TestSuiteSkippedException
;
use Zynga\Framework\ReflectionCache\V1\ReflectionClasses;
use Zynga\Framework\Testing\TestCase\V2\Base as ZyngaTestCaseBase;
use Zynga\PHPUnit\V2\Interfaces\TestInterface;
use Zynga\PHPUnit\V2\TestCase;
use Zynga\PHPUnit\V2\TestResult;
use Zynga\PHPUnit\V2\TestSuite;

/**
 * A TestSuite is a composite of Tests. It runs a collection of test cases.
 *
 * Here is an example using the dynamic test definition.
 *
 * <code>
 * <?php
 * $suite = new PHPUnit_Framework_TestSuite;
 * $suite->addTest(new MathTest('testPass'));
 * ?>
 * </code>
 *
 * Alternatively, a TestSuite can extract the tests to be run automatically.
 * To do so you pass a ReflectionClass instance for your
 * TestCase class to the PHPUnit_Framework_TestSuite
 * constructor.
 *
 * <code>
 * <?php
 * $suite = new PHPUnit_Framework_TestSuite(
 *   ReflectionClasses::getReflection('MathTest')
 * );
 * ?>
 * </code>
 *
 * This constructor creates a suite with all the methods starting with
 * "test" that take no arguments.
 *
 * @since Class available since Release 2.0.0
 */

// JEO: removed iterator aggregate IteratorAggregate as it expectx <T>
class PHPUnit_Framework_TestSuite
 extends TestSuite
  implements TestInterface, PHPUnit_Framework_SelfDescribing {
  /**
   * Last count of tests in this suite.
   *
   * @var int|null
   */
  private $cachedNumTests;

  /**
   * Enable or disable the backup and restoration of the $GLOBALS array.
   *
   * @var bool
   */
  protected $backupGlobals = null;

  /**
   * Enable or disable the backup and restoration of static attributes.
   *
   * @var bool
   */
  protected $backupStaticAttributes = null;

  /**
   * @var bool
   */
  private $beStrictAboutChangesToGlobalState = null;

  /**
   * @var bool
   */
  protected $runTestInSeparateProcess = false;

  /**
   * The test groups of the test suite.
   *
   * @var array
   */
  protected $groups = [];


  /**
   * The number of tests in the test suite.
   *
   * @var int
   */
  protected $numTests = -1;

  /**
   * @var bool
   */
  protected $testCase = false;

  /**
   * @var array
   */
  protected $foundClasses = [];

  /**
   * @var PHPUnit_Runner_Filter_Factory
   */
  private $iteratorFilter = null;

  /**
   * Constructs a new TestSuite:
   *
   *   - PHPUnit_Framework_TestSuite() constructs an empty TestSuite.
   *
   *   - PHPUnit_Framework_TestSuite(ReflectionClass) constructs a
   *     TestSuite from the given class.
   *
   *   - PHPUnit_Framework_TestSuite(ReflectionClass, String)
   *     constructs a TestSuite from the given class with the given
   *     name.
   *
   *   - PHPUnit_Framework_TestSuite(String) either constructs a
   *     TestSuite from the given class (if the passed string is the
   *     name of an existing class) or constructs an empty TestSuite
   *     with the given name.
   *
   * @param mixed  $theClass
   * @param string $name
   *
   * @throws PHPUnit_Framework_Exception
   */
  public function __construct($theClass = '', $name = '') {

    parent::__construct();

    $argumentsValid = false;

    if (is_object($theClass) && $theClass instanceof ReflectionClass) {
      $argumentsValid = true;
      $name = $theClass->getName();
    } else if (is_string($theClass) &&
               $theClass !== '' &&
               class_exists($theClass, true)) {
      $argumentsValid = true;

      if ($name == '') {
        $name = $theClass;
      }

      $theClass = ReflectionClasses::getReflection($theClass);
    } else if (is_string($theClass)) {

      $this->setName($theClass);

      return;
    }

    if (!$argumentsValid) {
      throw new PHPUnit_Framework_Exception();
    }

    if (!$theClass->isSubClassOf(ZyngaTestCaseBase::class) &&
        !$theClass->isSubClassOf(TestCase::class)) {
      throw new PHPUnit_Framework_Exception(
        'Class "'.
        $theClass->name.
        '" does not extend ('.
        ZyngaTestCaseBase::class.
        ' or '.
        TestCase::class.
        ')',
      );
    }

    if ($name != '') {
      $this->setName($name);
    } else {
      $this->setName($theClass->getName());
    }

    $constructor = $theClass->getConstructor();

    if ($constructor !== null && !$constructor->isPublic()) {
      $this->addTest(
        self::warning(
          sprintf(
            'Class "%s" has no public constructor.',
            $theClass->getName(),
          ),
        ),
      );

      return;
    }

    foreach ($theClass->getMethods() as $method) {
      $this->addTestMethod($theClass, $method);
    }

    if ($this->getTestCount() == 0) {
      $this->addTest(
        self::warning(
          sprintf('No tests found in class "%s".', $theClass->getName()),
        ),
      );
    }

    $this->testCase = true;
  }

  public function setDependencies(Vector<string> $deps): bool {
    return true;
  }

  /**
   * Returns a string representation of the test suite.
   *
   * @return string
   */
  public function toString() {
    return $this->getName();
  }

  /**
   * Adds the tests from the given class to the suite.
   *
   * @param mixed $testClass
   *
   * @throws PHPUnit_Framework_Exception
   */
  public function addTestSuite($testClass) {
    if (is_string($testClass) && class_exists($testClass)) {
      $testClass = ReflectionClasses::getReflection($testClass);
    }

    if (!is_object($testClass)) {
      throw PHPUnit_Util_InvalidArgumentHelper::factory(
        1,
        'class name or object',
      );
    }

    if ($testClass instanceof self) {
      $this->addTest($testClass);
    } else if ($testClass instanceof ReflectionClass) {
      $suiteMethod = false;

      if (!$testClass->isAbstract()) {
        if ($testClass->hasMethod(
              PHPUnit_Runner_BaseTestRunner::SUITE_METHODNAME,
            )) {
          $method = $testClass->getMethod(
            PHPUnit_Runner_BaseTestRunner::SUITE_METHODNAME,
          );

          if ($method->isStatic()) {
            $this->addTest($method->invoke(null, $testClass->getName()));

            $suiteMethod = true;
          }
        }
      }

      if (!$suiteMethod && !$testClass->isAbstract()) {
        $this->addTest(new self($testClass));
      }

    } else {
      throw new PHPUnit_Framework_Exception();
    }
  }

  /**
   * Wraps both <code>addTest()</code> and <code>addTestSuite</code>
   * as well as the separate import statements for the user's convenience.
   *
   * If the named file cannot be read or there are no new tests that can be
   * added, a <code>PHPUnit_Framework_WarningTestCase</code> will be created instead,
   * leaving the current test run untouched.
   *
   * @param string $filename
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since Method available since Release 2.3.0
   */
  public function addTestFile($filename) {
    if (!is_string($filename)) {
      throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'string');
    }

    if (file_exists($filename) && substr($filename, -5) == '.phpt') {
      $this->addTest(new PHPUnit_Extensions_PhptTestCase($filename));

      return;
    }

    // The given file may contain further stub classes in addition to the
    // test class itself. Figure out the actual test class.
    $classes = get_declared_classes();
    $filename = PHPUnit_Util_Fileloader::checkAndLoad($filename);
    $newClasses = array_diff(get_declared_classes(), $classes);

    // The diff is empty in case a parent class (with test methods) is added
    // AFTER a child class that inherited from it. To account for that case,
    // cumulate all discovered classes, so the parent class may be found in
    // a later invocation.
    if (!empty($newClasses)) {
      // On the assumption that test classes are defined first in files,
      // process discovered classes in approximate LIFO order, so as to
      // avoid unnecessary reflection.
      $this->foundClasses = array_merge($newClasses, $this->foundClasses);
    }

    // The test class's name must match the filename, either in full, or as
    // a PEAR/PSR-0 prefixed shortname ('NameSpace_ShortName'), or as a
    // PSR-1 local shortname ('NameSpace\ShortName'). The comparison must be
    // anchored to prevent false-positive matches (e.g., 'OtherShortName').
    $shortname = basename($filename, '.php');
    $shortnameRegEx = '/(?:^|_|\\\\)'.preg_quote($shortname, '/').'$/';

    foreach ($this->foundClasses as $i => $className) {
      if (preg_match($shortnameRegEx, $className)) {
        $class = ReflectionClasses::getReflection($className);

        if ($class->getFileName() == $filename) {
          $newClasses = [$className];
          unset($this->foundClasses[$i]);
          break;
        }
      }
    }

    foreach ($newClasses as $className) {
      $class = ReflectionClasses::getReflection($className);

      if (!$class->isAbstract()) {
        if ($class->hasMethod(
              PHPUnit_Runner_BaseTestRunner::SUITE_METHODNAME,
            )) {
          $method = $class->getMethod(
            PHPUnit_Runner_BaseTestRunner::SUITE_METHODNAME,
          );

          if ($method->isStatic()) {
            $this->addTest($method->invoke(null, $className));
          }
        } else if ($class->implementsInterface(
                     'Zynga\PHPUnit\V2\Interfaces\TestInterface',
                   )) {
          $this->addTestSuite($class);
        }
      }
    }

    $this->numTests = -1;
  }

  /**
   * Wrapper for addTestFile() that adds multiple test files.
   *
   * @param array|Iterator $filenames
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since Method available since Release 2.3.0
   */
  public function addTestFiles($filenames) {
    if (!(is_array($filenames) ||
          (is_object($filenames) && $filenames instanceof Iterator))) {
      throw PHPUnit_Util_InvalidArgumentHelper::factory(
        1,
        'array or iterator',
      );
    }

    foreach ($filenames as $filename) {
      $this->addTestFile((string) $filename);
    }
  }

  // /**
  //  * Counts the number of test cases that will be run by this test.
  //  *
  //  * @param bool $preferCache Indicates if cache is preferred.
  //  *
  //  * @return int
  //  */
  // public function count($preferCache = false) {
  //
  //   if ($preferCache && $this->cachedNumTests !== null) {
  //     $numTests = $this->cachedNumTests;
  //   } else {
  //     $numTests = 0;
  //
  //     foreach ($this->tests() as $test) {
  //       $numTests += $test->count();
  //     }
  //
  //     $this->cachedNumTests = $numTests;
  //   }
  //
  //   return $numTests;
  // }

  public function getCount(bool $preferCache = false): int {
    return $this->count($preferCache);
  }

  /**
   * @param ReflectionClass $theClass
   * @param string          $name
   *
   * @return TestInterface
   *
   * @throws PHPUnit_Framework_Exception
   */
  public static function createTest(ReflectionClass $theClass, $name) {
    $className = $theClass->getName();

    if (!$theClass->isInstantiable()) {
      return
        self::warning(sprintf('Cannot instantiate class "%s".', $className));
    }

    $backupSettings = PHPUnit_Util_Test::getBackupSettings($className, $name);

    $preserveGlobalState =
      PHPUnit_Util_Test::getPreserveGlobalStateSettings($className, $name);

    $runTestInSeparateProcess =
      PHPUnit_Util_Test::getProcessIsolationSettings($className, $name);

    $constructor = $theClass->getConstructor();

    $test = null;

    if ($constructor !== null) {
      $parameters = $constructor->getParameters();
      // TestCase() or TestCase($name)
      if (count($parameters) < 2) {
        $test = new $className($name);
      } // TestCase($name, $data)
      else {
        $t = null;
        $data = null;
        try {
          $data = PHPUnit_Util_Test::getProvidedData($className, $name);
        } catch (IncompleteException $e) {

          $message = sprintf(
            'Test for %s::%s marked incomplete by data provider',
            $className,
            $name,
          );

          $_message = $e->getMessage();

          if (!empty($_message)) {
            $message .= "\n".$_message;
          }

          $data = self::incompleteTest($className, $name, $message);
        } catch (SkippedException $e) {

          $message = sprintf(
            'Test for %s::%s skipped by data provider',
            $className,
            $name,
          );

          $_message = $e->getMessage();

          if (!empty($_message)) {
            $message .= "\n".$_message;
          }

          $data = self::skipTest($className, $name, $message);
        } catch (Throwable $_t) {
          $t = $_t;
        } catch (Exception $_t) {
          $t = $_t;
        }

        if ($t instanceof Exception) {
          $message = sprintf(
            'The data provider specified for %s::%s is invalid.',
            $className,
            $name,
          );

          $_message = $t->getMessage();

          if (!empty($_message)) {
            $message .= "\n".$_message;
          }

          $data = self::warning($message);
        }

        // Test method with @dataProvider.
        if (isset($data)) {
          $test = new PHPUnit_Framework_TestSuite_DataProvider(
            $className.'::'.$name,
          );

          if (empty($data)) {
            $data = self::warning(
              sprintf('No tests found in suite "%s".', $test->getName()),
            );
          }

          $groups = PHPUnit_Util_Test::getGroups($className, $name);

          if ($data instanceof PHPUnit_Framework_WarningTestCase ||
              $data instanceof PHPUnit_Framework_SkippedTestCase ||
              $data instanceof PHPUnit_Framework_IncompleteTestCase) {
            $test->addTest($data, $groups);
          } else {
            foreach ($data as $_dataName => $_data) {
              // JEO: At times $_dataName can be a int somehow? Let's coerce it to string.
              $_test = new $className($name, $_data, strval($_dataName));

              if ($runTestInSeparateProcess) {
                $_test->setRunTestInSeparateProcess(true);

                if ($preserveGlobalState !== null) {
                  $_test->setPreserveGlobalState($preserveGlobalState);
                }
              }

              if ($backupSettings['backupGlobals'] !== null) {
                $_test->setBackupGlobals($backupSettings['backupGlobals']);
              }

              if ($backupSettings['backupStaticAttributes'] !== null) {
                $_test->setBackupStaticAttributes(
                  $backupSettings['backupStaticAttributes'],
                );
              }

              $test->addTest($_test, $groups);
            }
          }
        } else {
          $test = new $className($name);
        }
      }
    }

    if (!isset($test)) {
      throw new PHPUnit_Framework_Exception('No valid test provided.');
    }

    if ($test instanceof ZyngaTestCaseBase || $test instanceof TestCase) {
      $test->setName($name);

      if ($runTestInSeparateProcess) {
        $test->setRunTestInSeparateProcess(true);

        if ($preserveGlobalState !== null) {
          $test->setPreserveGlobalState($preserveGlobalState);
        }
      }

      if ($backupSettings['backupGlobals'] !== null) {
        $test->setBackupGlobals($backupSettings['backupGlobals']);
      }

      if ($backupSettings['backupStaticAttributes'] !== null) {
        $test->setBackupStaticAttributes(
          $backupSettings['backupStaticAttributes'],
        );
      }
    }

    return $test;
  }

  /**
   * Creates a default TestResult object.
   *
   * @return TestResult
   */
  protected function createResult(): TestResult {
    return new TestResult();
  }

  /**
   * Set tests groups of the test case
   *
   * @param array $groups
   *
   * @since Method available since Release 4.0.0
   */
  public function setGroupDetails(array $groups) {
    $this->groups = $groups;
  }

  /**
   * Runs the tests and collects their result in a TestResult.
   *
   * @param TestResult $result
   *
   * @return TestResult
   */
  public function run(?TestResult $result = null) {
    if ($result === null) {
      $result = $this->createResult();
    }

    if ($this->getCount() == 0) {
      return $result;
    }

    $hookMethods = PHPUnit_Util_Test::getHookMethods($this->getName());

    $result->startTestSuite($this);

    // JEO: Quick get the first and last tests off the stack.
    $firstTest = null;
    $lastTest = null;

    foreach ($this->getIterator() as $test) {
      if ($firstTest === null) {
        $firstTest = $test;
      }
      $lastTest = $test;
    }

    try {
      $this->setUp();

      foreach ($hookMethods['beforeClass'] as $beforeClassMethod) {
        if ($this->testCase === true &&
            class_exists($this->getName(), false) &&
            method_exists($this->getName(), $beforeClassMethod)) {

          $missingRequirements = PHPUnit_Util_Test::getMissingRequirements(
            $this->getName(),
            $beforeClassMethod,
          );

          if (count($missingRequirements) > 0) {
            $this->markTestSuiteSkipped(
              implode(PHP_EOL, $missingRequirements),
            );
          }

        }
      }

      // JEO: Run the doSetupBeforeClass or the older setUpBeforeClass on the first test.
      if (method_exists($firstTest, 'doSetUpBeforeClass')) {
        $firstTest->doSetUpBeforeClass();
      } else if (method_exists($firstTest, 'setUpBeforeClass')) {
        $firstTest->setUpBeforeClass();
      }

    } catch (TestSuiteSkippedException $e) {
      $numTests = $this->getCount();

      for ($i = 0; $i < $numTests; $i++) {
        $result->startTest($this);
        $result->addFailure($this, $e, 0.0);
        $result->endTest($this, 0.0);
      }

      $this->tearDown();
      $result->endTestSuite($this);

      return $result;
    } catch (Throwable $_t) {
      $t = $_t;
    } catch (Exception $_t) {
      $t = $_t;
    }

    if (isset($t)) {
      $numTests = $this->getCount();

      for ($i = 0; $i < $numTests; $i++) {
        $result->startTest($this);
        $result->addError($this, $t, 0.0);
        $result->endTest($this, 0.0);
      }

      $this->tearDown();
      $result->endTestSuite($this);

      return $result;
    }

    foreach ($this->getIterator() as $test) {

      if ($result->shouldStop()) {
        break;
      }

      $test->run($result);

    }

    // JEO: Run the doTearDownAfterClass
    if (method_exists($lastTest, 'doTearDownAfterClass')) {
      $lastTest->doTearDownAfterClass();
    } else if (method_exists($lastTest, 'tearDownAfterClass')) {
      $lastTest->tearDownAfterClass();
    }

    $this->tearDown();

    $result->endTestSuite($this);

    return $result;
  }

  /**
   * @param bool $runTestInSeparateProcess
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since Method available since Release 3.7.0
   */
  public function setRunTestInSeparateProcess($runTestInSeparateProcess) {
    if (is_bool($runTestInSeparateProcess)) {
      $this->runTestInSeparateProcess = $runTestInSeparateProcess;
    } else {
      throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'boolean');
    }
  }

  /**
   * Runs a test.
   *
   * @deprecated
   *
   * @param TestInterface       $test
   * @param TestResult $result
   */
  public function runTest(TestInterface $test, TestResult $result) {
    $test->run($result);
  }

  /**
   * Mark the test suite as skipped.
   *
   * @param string $message
   *
   * @throws TestSuiteSkippedException
   *
   * @since Method available since Release 3.0.0
   */
  public function markTestSuiteSkipped($message = '') {
    throw new TestSuiteSkippedException($message);
  }

  /**
   * @param ReflectionClass  $class
   * @param ReflectionMethod $method
   */
  protected function addTestMethod(
    ReflectionClass $class,
    ReflectionMethod $method,
  ) {
    if (!$this->isTestMethod($method)) {
      return;
    }

    $name = $method->getName();

    if (!$method->isPublic()) {
      $this->addTest(
        self::warning(
          sprintf(
            'Test method "%s" in test class "%s" is not public.',
            $name,
            $class->getName(),
          ),
        ),
      );

      return;
    }

    $test = self::createTest($class, $name);

    if (($test instanceof ZyngaTestCaseBase || $test instanceof TestCase) ||
        $test instanceof PHPUnit_Framework_TestSuite_DataProvider) {

      $deps = PHPUnit_Util_Test::getDependencies($class->getName(), $name);

      $v_deps = Vector {};

      if (is_array($deps) && count($deps) > 0) {
        $v_deps->addAll($deps);
      }

      $test->setDependencies($v_deps);
    }

    $this->addTest(
      $test,
      PHPUnit_Util_Test::getGroups($class->getName(), $name),
    );
    
  }

  /**
   * @param ReflectionMethod $method
   *
   * @return bool
   */
  public static function isTestMethod(ReflectionMethod $method) {
    if (strpos($method->name, 'test') === 0) {
      return true;
    }

    // @scenario on TestCase::testMethod()
    // @test     on TestCase::testMethod()
    $docComment = $method->getDocComment();

    return
      strpos($docComment, '@test') !== false ||
      strpos($docComment, '@scenario') !== false;
  }

  /**
   * @param string $message
   *
   * @return PHPUnit_Framework_WarningTestCase
   */
  protected static function warning($message) {
    return new PHPUnit_Framework_WarningTestCase($message);
  }

  /**
   * @param string $class
   * @param string $methodName
   * @param string $message
   *
   * @return PHPUnit_Framework_SkippedTestCase
   *
   * @since Method available since Release 4.3.0
   */
  protected static function skipTest($class, $methodName, $message) {
    return
      new PHPUnit_Framework_SkippedTestCase($class, $methodName, $message);
  }

  /**
   * @param string $class
   * @param string $methodName
   * @param string $message
   *
   * @return PHPUnit_Framework_IncompleteTestCase
   *
   * @since Method available since Release 4.3.0
   */
  protected static function incompleteTest($class, $methodName, $message) {
    return
      new PHPUnit_Framework_IncompleteTestCase($class, $methodName, $message);
  }

  /**
   * @param bool $beStrictAboutChangesToGlobalState
   *
   * @since Method available since Release 4.6.0
   */
  public function setbeStrictAboutChangesToGlobalState(
    $beStrictAboutChangesToGlobalState,
  ) {
    if (is_null($this->beStrictAboutChangesToGlobalState) &&
        is_bool($beStrictAboutChangesToGlobalState)) {
      $this->beStrictAboutChangesToGlobalState =
        $beStrictAboutChangesToGlobalState;
    }
  }

  /**
   * @param bool $backupGlobals
   *
   * @since Method available since Release 3.3.0
   */
  public function setBackupGlobals($backupGlobals) {
    if (is_null($this->backupGlobals) && is_bool($backupGlobals)) {
      $this->backupGlobals = $backupGlobals;
    }
  }

  /**
   * @param bool $backupStaticAttributes
   *
   * @since Method available since Release 3.4.0
   */
  public function setBackupStaticAttributes($backupStaticAttributes) {
    if (is_null($this->backupStaticAttributes) &&
        is_bool($backupStaticAttributes)) {
      $this->backupStaticAttributes = $backupStaticAttributes;
    }
  }

  /**
   * Returns an iterator for this test suite.
   *
   * @return RecursiveIteratorIterator
   *
   * @since Method available since Release 3.1.0
   */
  public function getIterator() {
    $iterator = new PHPUnit_Util_TestSuiteIterator($this);

    if ($this->iteratorFilter !== null) {
      $iterator = $this->iteratorFilter->factory($iterator, $this);
      // echo "picked up iterator=" . get_class($iterator) . "\n";
    }

    return $iterator;
  }

  public function injectFilter(PHPUnit_Runner_Filter_Factory $filter) {
    $this->iteratorFilter = $filter;
    foreach ($this->tests() as $test) {
      if ($test instanceof self) {
        // echo "injecting! to=" . get_class($test) . ' name=' . $this->getName() . "\n";
        $test->injectFilter($filter);
      }
    }
  }

}
