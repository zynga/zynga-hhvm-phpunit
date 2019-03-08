<?hh // strict

namespace Zynga\PHPUnit\V2;

use Zynga\Framework\ReflectionCache\V1\ReflectionClasses;
use Zynga\Framework\Testing\TestCase\V2\Base as ZyngaTestCaseBase;

use Zynga\PHPUnit\V2\Interfaces\TestInterface;
use Zynga\PHPUnit\V2\TestResult;

use \ReflectionClass;

class TestSuite implements TestInterface {

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

  public function __construct() {
    $this->_name = '';
    $this->_tests = Vector {};
    $this->_groups = Map {};
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
  public function setName(string $name): void {
    $this->_name = $name;
  }

  final public function setGroups(Vector<string> $groups): void {
    foreach ($groups as $group) {
      $this->_groups->set($group, Vector {});
    }
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

    foreach ($this->_tests as $test) {
      $numTests += $test->count();
    }

    return $numTests;

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

    $reflection = ReflectionClasses::getReflection($test);

    if ($reflection instanceof ReflectionClass &&
        !$reflection->isAbstract()) {

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

  }

  public function run(?TestResult $result = null): TestResult {
    return new TestResult();
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
