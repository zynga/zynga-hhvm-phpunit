<?hh // strict

namespace Zynga\PHPUnit\V2\Test;

use Zynga\PHPUnit\V2\Annotations;
use Zynga\PHPUnit\V2\Interfaces\TestInterface;
use Zynga\PHPUnit\V2\TestResult;

/**
 * Base class for both TestClass and TestSuite as there is some functionality
 *  overlap that we shouldn't maintain in two places.
 */
abstract class Base implements TestInterface {

  private TestResult $_result;

  public function __construct() {
    $this->_result = new TestResult();
  }

  final public function setResult(TestResult $result): bool {
    $this->_result = $result;
    return true;
  }

  final public function getResult(): TestResult {
    return $this->_result;
  }

  /**
   * Returns the annotations for this test.
   *
   * @return array
   *
   * @since Method available since Release 3.4.0
   */
  final public function getAnnotations(
  ): Map<string, Map<string, Vector<string>>> {

    $className = $this->getClass();
    $methodName = $this->getName();

    return Annotations::parseTestMethodAnnotations($className, $methodName);

  }

  final public function getAllAnnotationsForKey(string $key): Vector<string> {

    $className = $this->getClass();
    $methodName = $this->getName();

    return
      Annotations::getAllAnnotationsForKey($key, $className, $methodName);

  }

  final public function getAnnotationsForKey(
    string $context,
    string $key,
  ): Vector<string> {

    $className = $this->getClass();
    $methodName = $this->getName();

    return Annotations::getAnnotationsForKey(
      $context,
      $key,
      $className,
      $methodName,
    );

  }

  final public function getGroupsFromAnnotation(): Vector<string> {
    $className = $this->getClass();
    $methodName = $this->getName();
    return $this->_getGroupsFromAnnotation($className, $methodName);
  }

  private function _getGroupsFromAnnotation(
    string $className,
    string $methodName,
  ): Vector<string> {

    $groups = Vector {'default'};

    $authors =
      Annotations::getAllAnnotationsForKey('author', $className, $methodName);
    $groups->addAll($authors);

    $userDefinedGroups =
      Annotations::getAllAnnotationsForKey('group', $className, $methodName);
    $groups->addAll($userDefinedGroups);

    $tickets =
      Annotations::getAllAnnotationsForKey('ticket', $className, $methodName);
    $groups->addAll($tickets);

    $large =
      Annotations::getAllAnnotationsForKey('large', $className, $methodName);
    $medium =
      Annotations::getAllAnnotationsForKey('medium', $className, $methodName);
    $small =
      Annotations::getAllAnnotationsForKey('small', $className, $methodName);

    if ($large->count() > 1) {
      $groups->add('large');
    } else if ($medium->count() > 1) {
      $groups->add('medium');
    } else if ($small->count() > 1) {
      $groups->add('small');
    }

    return $groups;

  }

  final public function getHookMethods(): Map<string, Vector<string>> {

    // --
    // Before the test / test suite is started
    // --
    $beforeClassMethods = $this->mergeHooks(
      Vector {'setUpBeforeClass', 'doSetUpBeforeClass'},
      //Annotations::get
      $this->getAllAnnotationsForKey('beforeClass'),
    );

    $afterClassMethods = $this->mergeHooks(
      Vector {'tearDownAfterClass', 'doTearDownAfterClass'},
      $this->getAllAnnotationsForKey('afterClass'),
    );

    // --
    // This is a per test before / after callset
    // --
    $beforeMethods = $this->mergeHooks(
      Vector {'setUp'},
      $this->getAllAnnotationsForKey('before'),
    );

    $afterMethods = $this->mergeHooks(
      Vector {'tearDown'},
      $this->getAllAnnotationsForKey('after'),
    );

    $hooks = Map {
      'beforeClass' => $beforeClassMethods,
      'before' => $beforeMethods,
      'after' => $afterMethods,
      'afterClass' => $afterClassMethods,
    };

    return $hooks;

  }

  final public function mergeHooks(
    Vector<string> $template,
    Vector<string> $userDefined,
  ): Vector<string> {

    $combinedMap = Vector {};
    $combinedMap->addAll($template);
    $combinedMap->addAll($userDefined);

    return $combinedMap;

  }

}
