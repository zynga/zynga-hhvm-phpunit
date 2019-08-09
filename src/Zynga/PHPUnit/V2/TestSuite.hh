<?hh // strict

namespace Zynga\PHPUnit\V2;

use Zynga\PHPUnit\V2\Exceptions\InvalidArgumentExceptionFactory;
use Zynga\PHPUnit\V2\Exceptions\TestSuite\TestCaseExtensionException;
use
  Zynga\PHPUnit\V2\Exceptions\TestSuiteError\SkippedException as TestSuiteSkippedException
;

use Zynga\Framework\Dynamic\V1\DynamicMethodCall;
use Zynga\Framework\ReflectionCache\V1\ReflectionClasses;
use Zynga\Framework\Testing\TestCase\V2\Base as ZyngaTestCaseBase;

use Zynga\PHPUnit\V2\FileLoader;
use Zynga\PHPUnit\V2\Filter\Factory as FilterFactory;
use Zynga\PHPUnit\V2\Interfaces\TestInterface;
use Zynga\PHPUnit\V2\Test\Base;
use Zynga\PHPUnit\V2\Test\Requirements;
use Zynga\PHPUnit\V2\TestResult;
use Zynga\PHPUnit\V2\TestSuite\DataProvider;
use Zynga\PHPUnit\V2\TestSuite\StaticUtil;
use Zynga\PHPUnit\V2\TestSuiteIterator;
use Zynga\PHPUnit\V2\TestSuite\OnTestClassChangeListener;
use Zynga\PHPUnit\V2\Exceptions\TestSuite\TestCaseNotFoundException;
use Zynga\PHPUnit\V2\Exceptions\TestSuite\TestMethodHiddenException;

use \Exception;
use \RecursiveIterator;
use \ReflectionClass;
use \ReflectionMethod;

class TestSuite extends Base {

  const string SUITE_METHODNAME = 'suite';

  /**
   * The name of the test suite.
   *
   * @var string
   */
  private string $_name = '';

  /**
   * The tests in the test suite.
   */
  private Vector<TestInterface> $_tests;

  /**
   * The groups that this test suite supports, and it's constiuiant tests.
   */
  private Map<string, Vector<TestInterface>> $_groups;

  /**
   * @var PHPUnit_Runner_Filter_Factory
   */
  //private ?FilterFactory $_iteratorFilter;

  /**
   * Constructs a new TestSuite:
   *
   *   - TestSuite() constructs an empty TestSuite.
   *
   *   - TestSuite(ReflectionClass) constructs a
   *     TestSuite from the given class.
   *
   *   - TestSuite(ReflectionClass, String)
   *     constructs a TestSuite from the given class with the given
   *     name.
   *
   *   - TestSuite(String) either constructs a
   *     TestSuite from the given class (if the passed string is the
   *     name of an existing class) or constructs an empty TestSuite
   *     with the given name.
   *
   * @param mixed  $theClass
   * @param string $name
   *
   * @throws PHPUnit_Framework_Exception
   */
  public function __construct(mixed $theClass = '', string $name = '') {

    parent::__construct();

    $this->_name = '';
    $this->_tests = Vector {};
    $this->_groups = Map {};

    //$this->_iteratorFilter = null;

    $argumentsValid = false;

    // --
    // Supports callables as either:
    // 1) [ReflectionClass, null]
    // 2) [string validClassName, string optionalName defaults to validClassName]
    if ($theClass instanceof ReflectionClass) {
      $this->_name = $theClass->getName();
    } else if (is_string($theClass) &&
               $theClass !== '' &&
               class_exists($theClass, true)) {
      $theClass = ReflectionClasses::getReflection($theClass);
    } else if ($theClass == '') {
      // This is a stub suite | testcase that doesn't actually have a real class implementation.
      $this->_name = get_class($this);
      return;
    } else if (is_string($theClass)) {

      $result = $this->getResult();

      // Off to a bad start here.
      $result->addError(
        $this,
        new TestCaseNotFoundException('class='.$theClass.' is not found.'),
        0.0,
      );

      return;

    }

    if (!$theClass instanceof ReflectionClass) {

      throw InvalidArgumentExceptionFactory::factory(
        0,
        'ReflectionClass or validClassName',
        $theClass,
      );

    }

    if (!$theClass->isSubclassOf(ZyngaTestCaseBase::class) &&
        !$theClass->isSubclassOf(TestCase::class)) {

      $result = $this->getResult();

      $result->addError(
        $this,
        new TestCaseExtensionException(
          'class='.
          $theClass->name.
          ' does not extend ('.
          ZyngaTestCaseBase::class.
          ' or '.
          TestCase::class.
          ')',
        ),
        0.0,
      );

      return;

    }

    if ($name != '') {
      $this->setName($name);
    } else {
      $this->setName($theClass->getName());
    }

    $constructor = $theClass->getConstructor();

    if ($constructor !== null && !$constructor->isPublic()) {
      $this->addTest(
        StaticUtil::warning(
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
        StaticUtil::warning(
          sprintf('No tests found in class "%s".', $theClass->getName()),
        ),
      );
    }

  }

  /**
   * Returns a string representation of the test suite.
   *
   * @return string
   */
  final public function toString(): string {
    return $this->getName();
  }

  final public function getClass(): string {
    return get_class($this);
  }

  /**
   * Returns the name of the suite.
   *
   * @return string
   */
  public function getName(): string {
    return $this->_name;
  }

  /**
   * Sets the name of the suite.
   *
   * @param  string
   */
  public function setName(string $name): bool {
    $this->_name = $name;
    return true;
  }

  final public function setGroupsFromAnnotation(): bool {
    $groups = $this->getGroupsFromAnnotation();
    $this->setGroups($groups);
    return true;
  }

  final public function setGroups(Vector<string> $groups): void {
    foreach ($groups as $group) {
      $this->_groups->set($group, Vector {});
    }
  }

  public function setDependenciesFromAnnotation(): bool {
    // noop by default.
    return true;
  }

  // Note: Overloaded for the dataprovider.
  public function setDependencies(Vector<string> $deps): bool {
    // noop by default.
    return true;
  }

  /**
   * Set tests groups of the test case
   *
   * @param array $groups
   *
   * @since Method available since Release 4.0.0
   */
  final public function setGroupDetails(
    Map<string, Vector<TestInterface>> $groups,
  ): void {
    $this->_groups = $groups;
  }

  /**
   * Returns the test groups of the suite.
   *
   * @return Vector<string>
   *
   * @since Method available since Release 3.2.0
   */
  final public function getGroups(): Vector<string> {
    return $this->_groups->keys();
  }

  final public function getGroupDetails(): Map<string, Vector<TestInterface>> {
    return $this->_groups;
  }

  final public function count(): int {

    $numTests = 0;

    // foreach ($this->_tests as $test) {
    //   $numTests += $test->count();
    // }

    // --
    // JEO: We need to support both test count() and testSuite count which
    //      requires iterating to children to find them all.
    // --
    foreach ($this->getIterator()->getChildren() as $child) {
      $numTests += $child->count();
    }

    return $numTests;

  }

  final public function getCount(): int {

    return $this->count();
  }

  final public function getTestCount(): int {
    return $this->_tests->count();
  }

  /**
   * Returns the test at the given index.
   *
   * @param  int
   *
   * @return TestInterface|false
   */
  final public function testAt(int $index): mixed {
    $test = $this->_tests->get($index);
    if ($test instanceof TestInterface) {
      return $test;
    }
    return false;
  }

  /**
   * Returns the tests as an enumeration.
   *
   */
  final public function tests(): Vector<TestInterface> {
    return $this->_tests;
  }

  /**
   * Set tests of the test suite
   *
   * @param Vector<TestInterface> $tests
   *
   * @since Method available since Release 4.0.0
   */
  final public function setTests(Vector<TestInterface> $tests): void {
    $this->_tests = $tests;
  }

  /**
   * Adds a test to the suite.
   *
   * @param TestInterface $test
   * @param array                  $groups
   */
  final public function addTest(
    TestInterface $test,
    mixed $groups = null,
  ): void {

    $vecGroups = Vector {};

    if ($groups == null) {
      $vecGroups->add('default');
    } else if (is_array($groups)) {
      $vecGroups->addAll($groups);
    } else if ($groups instanceof Vector) {
      $vecGroups = $groups;
    }

    $this->_tests->add($test);

    // @TODO: looking at not forward porting this value.
    // $this->numTests = -1;

    if ($test instanceof TestSuite && $vecGroups->count() == 0) {
      $groups = $test->getGroups();
    }

    if ($vecGroups->count() == 0) {
      $vecGroups = Vector {'default'};
    }

    foreach ($vecGroups as $group) {

      $existingGroup = $this->_groups->get($group);

      if ($existingGroup instanceof Vector) {
        $existingGroup->add($test);
      } else {
        $existingGroup = Vector {$test};
        $this->_groups->set($group, $existingGroup);
      }

    }

    if ($test instanceof ZyngaTestCaseBase) {
      $test->setGroups($groups);
    } else if ($test instanceof TestCase) {
      $test->setGroups($groups);
    }

  }

  /**
   * Adds the tests from the given class to the suite.
   *
   * @param mixed $testClass
   *
   * @throws PHPUnit_Framework_Exception
   */
  public function addTestSuite(mixed $testClass): void {

    if (is_string($testClass) && class_exists($testClass)) {
      $testClass = ReflectionClasses::getReflection($testClass);
    }

    if (!is_object($testClass)) {
      throw InvalidArgumentExceptionFactory::factory(
        1,
        'class name or object',
      );
    }

    if ($testClass instanceof self) {
      $this->addTest($testClass);
    } else if ($testClass instanceof ReflectionClass) {
      $suiteMethod = false;

      if (!$testClass->isAbstract()) {
        if ($testClass->hasMethod(self::SUITE_METHODNAME)) {
          $method = $testClass->getMethod(self::SUITE_METHODNAME);
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
      throw InvalidArgumentExceptionFactory::factory(
        1,
        'Failed to find a suitable test from arg',
      );
    }
  }

  /**
   * @param ReflectionClass  $class
   * @param ReflectionMethod $method
   */
  protected function addTestMethod(
    ReflectionClass $class,
    ReflectionMethod $method,
  ): void {

    if (!$this->isTestMethod($method)) {
      return;
    }

    $name = $method->getName();

    if (!$method->isPublic()) {

      $result = $this->getResult();

      $result->addWarning(
        $this,
        new TestMethodHiddenException(
          sprintf(
            'Test method "%s" in test class "%s" is not public.',
            $name,
            $class->getName(),
          ),
        ),
        0.0,
      );

      return;
    }

    $test = StaticUtil::createTest($class, $name);
    $test->setDependenciesFromAnnotation();
    $test->setGroupsFromAnnotation();

    $this->addTest($test, $test->getGroups());

  }

  /**
   * Returns an iterator for this test suite.
   *
   * @return RecursiveIteratorIterator
   *
   * @since Method available since Release 3.1.0
   */
  final public function getIterator(): TestSuiteIterator {

    $iterator = new TestSuiteIterator($this);

    // @TODO: We need to reimplement test filtering, as I am not happy with it atm.
    //if ($this->_iteratorFilter !== null) {
    //  $iterator = $this->_iteratorFilter->factory($iterator, $this);
    //  // echo "picked up iterator=" . get_class($iterator) . "\n";
    //}

    return $iterator;
  }

  // @TODO: filter injection is not in a great place atm, would like to reimplement it.
  // public function injectFilter(PHPUnit_Runner_Filter_Factory $filter): void {
  //   $this->_iteratorFilter = $filter;
  //   foreach ($this->tests() as $test) {
  //     if ($test instanceof self) {
  //       // echo "injecting! to=" . get_class($test) . ' name=' . $this->getName() . "\n";
  //       $test->injectFilter($filter);
  //     }
  //   }
  // }

  /**
   * @param ReflectionMethod $method
   *
   * @return bool
   */
  public function isTestMethod(ReflectionMethod $method): bool {

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

  private function suiteFailedMarkAllTestsFailed(
    TestResult $result,
    Exception $e,
  ): TestResult {
    $numTests = $this->getCount();

    for ($i = 0; $i < $numTests; $i++) {
      $result->startTest($this);
      $result->addError($this, $e, 0.0);
      $result->endTest($this, 0.0);
    }

    $this->tearDown();
    $result->endTestSuite($this);

    return $result;
  }

  /**
   * Runs the tests and collects their result in a TestResult.
   *
   * @param TestResult $result
   *
   * @return TestResult
   */
  public function run(?TestResult $result = null): TestResult {

    if ($result instanceof TestResult) {
      $this->setResult($result);
    } else {
      $result = $this->getResult();
    }

    if ($this->getCount() == 0) {
      return $result;
    }

    $t = null;

    $suiteHooks = $this->getHookMethods();

    $result->startTestSuite($this);

    // JEO: Quick get the first and last tests off the stack.
    $firstTest = null;
    $lastTest = null;

    foreach ($this->getIterator()->getChildren() as $test) {
      if ($firstTest === null) {
        $firstTest = $test;
      }
      $lastTest = $test;
    }

    try {
      $this->setUp();

      // --
      // JEO: Take care of the requirements before we run any of the pre-class methods.
      // --
      foreach ($suiteHooks['beforeClass'] as $beforeClassMethod) {

        if (class_exists($this->getName(), false) &&
            method_exists($this->getName(), $beforeClassMethod)) {

          $missingRequirements = Requirements::getMissingRequirements($this);

          if (count($missingRequirements) > 0) {
            $this->markTestSuiteSkipped(
              implode(PHP_EOL, $missingRequirements),
            );
          }

        }

      }

    } catch (TestSuiteSkippedException $skip) {
      return $this->suiteFailedMarkAllTestsFailed($result, $skip);
    } catch (Exception $e) {
      return $this->suiteFailedMarkAllTestsFailed($result, $e);
    }

    foreach ($this->getIterator()->getChildren() as $test) {

      $didClassChange = OnTestClassChangeListener::isClassChange($test);

      if ($didClassChange === true) {

        // var_dump("CLASS_CHANGE=".get_class($test));

        // handle the afterClass method for the outgoing test first.
        list($afterOk, $afterException) =
          OnTestClassChangeListener::handleAfterClass();

        // Setup for a new class to run.
        OnTestClassChangeListener::clear();

        // Time for the incoming class to get it's day in the sun.
        //OnTestClassChangeListener::handleRequirements();
        list($beforeOk, $beforeException) =
          OnTestClassChangeListener::handleBeforeClass($test);

        if ($beforeOk !== true && $beforeException instanceof Exception) {
          return
            $this->suiteFailedMarkAllTestsFailed($result, $beforeException);
        }

      }

      if ($result->shouldStop()) {
        break;
      }

      $test->run($result);

      if ($didClassChange === true) {
        OnTestClassChangeListener::setClass($test);
      }

    }

    // handle outgoing tests, happens if we didn't have a class change.
    OnTestClassChangeListener::handleAfterClass();

    // Now clear the change listener, as we are done.
    OnTestClassChangeListener::clear();

    // JEO: Run the doTearDownAfterClass
    // foreach ($hookMethods['afterClass'] as $afterClassMethod) {
    //   DynamicMethodCall::callMethodOnObject(
    //     $lastTest,
    //     $afterClassMethod,
    //     Vector {},
    //     false,
    //     true,
    //   );
    // }

    $this->tearDown();

    $result->endTestSuite($this);

    return $result;
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
  final public function markTestSuiteSkipped(string $message = ''): void {
    throw new TestSuiteSkippedException($message);
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
  public function addTestFile(string $filename): void {

    // The given file may contain further stub classes in addition to the
    // test class itself. Figure out the actual test class.
    $newClasses = FileLoader::checkAndLoad($filename);

    foreach ($newClasses as $className) {

      $class = ReflectionClasses::getReflection($className);

      if (!$class instanceof ReflectionClass) {
        continue;
      }

      if (!$class->isAbstract()) {
        if ($class->hasMethod(self::SUITE_METHODNAME)) {
          $method = $class->getMethod(self::SUITE_METHODNAME);

          if ($method->isStatic()) {
            // echo "addTest=" . $className . "\n";
            $this->addTest($method->invoke(null, $className));
          }
        } else if ($class->implementsInterface(
                     'Zynga\PHPUnit\V2\Interfaces\TestInterface',
                   )) {
          // echo "addTestSuite=" . $className . "\n";
          $this->addTestSuite($class);
        }
      }
    }

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
  public function addTestFiles(Vector<string> $filenames): void {

    foreach ($filenames as $filename) {
      $this->addTestFile($filename);
    }

  }

  /**
   * Template Method that is called before the tests
   * of this test suite are run.
   *
   * @since Method available since Release 3.1.0
   */
  public function setUp(): void {}

  /**
   * Template Method that is called after the tests
   * of this test suite have finished running.
   *
   * @since Method available since Release 3.1.0
   */
  public function tearDown(): void {}

}
