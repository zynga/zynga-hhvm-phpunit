<?hh // strict

namespace Zynga\PHPUnit\V2\TestSuite;

use SebastianBergmann\PHPUnit\Exceptions\Exception as PHPUnitException;
use SebastianBergmann\PHPUnit\Exceptions\TestError\IncompleteException;
use SebastianBergmann\PHPUnit\Exceptions\TestError\SkippedException;
use Zynga\PHPUnit\V2\Annotations;
use Zynga\PHPUnit\V2\IncompleteTestCase;
use Zynga\PHPUnit\V2\Interfaces\TestInterface;
use Zynga\PHPUnit\V2\SkippedTestCase;
use Zynga\PHPUnit\V2\TestSuite\DataProvider;
use Zynga\PHPUnit\V2\TestSuite\DataProvider\Loader;
use Zynga\PHPUnit\V2\WarningTestCase;

use \Exception;
use \ReflectionClass;

class StaticUtil {

  /**
   * @param string $message
   *
   * @return PHPUnit_Framework_WarningTestCase
   */
  final public static function warning(string $message): WarningTestCase {
    return new WarningTestCase($message);
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
  final public static function skipTest(
    string $class,
    string $methodName,
    string $message,
  ): SkippedTestCase {
    return new SkippedTestCase($class, $methodName, $message);
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
  final public static function incompleteTest(
    string $class,
    string $methodName,
    string $message,
  ): IncompleteTestCase {
    return new IncompleteTestCase($class, $methodName, $message);
  }

  final public static function createTest_Simple(
    ReflectionClass $theClass,
    string $name,
    Vector<mixed> $args,
  ): TestInterface {

    // TestCase() or TestCase($name)
    $test = $theClass->newInstanceArgs($args);

    if ($test instanceof TestInterface) {
      return $test;
    }

    throw new Exception(
      'Failed to create test name='.$name.' class='.$theClass->getName(),
    );

  }

  final public static function createTest_LoadData(
    ReflectionClass $theClass,
    string $name,
  ): mixed {

    $t = null;
    $className = $theClass->getName();

    try {
      $data = Loader::getProvidedData($className, $name);
    } catch (IncompleteException $e) {

      $message = sprintf(
        'Test for %s::%s marked incomplete by data provider',
        $className,
        $name,
      );

      $_message = $e->getMessage();

      if ($_message != '') {
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

      if ($_message != '') {
        $message .= "\n".$_message;
      }

      $data = self::skipTest($className, $name, $message);
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

      if ($_message != '') {
        $message .= "\n".$_message;
      }

      $data = self::warning($message);
    }
  }

  /**
   * @param ReflectionClass $theClass
   * @param string          $name
   *
   * @return TestInterface
   *
   * @throws PHPUnit_Framework_Exception
   */
  final public static function createTest(
    ReflectionClass $theClass,
    string $name,
  ): TestInterface {

    $className = $theClass->getName();

    if (!$theClass->isInstantiable()) {
      return
        self::warning(sprintf('Cannot instantiate class "%s".', $className));
    }

    $constructor = $theClass->getConstructor();

    $test = null;

    if ($constructor !== null) {

      $parameters = $constructor->getParameters();

      if (count($parameters) < 2) {
        $test = self::createTest_Simple($theClass, $name, Vector {$name});
      } else {
        // Support for dataprovider based loading.
        // TestCase($name, $data)
        $data = self::createTest_LoadData($theClass, $name);

        // Test method with @dataProvider.
        if ($data != null) {

          // JEO: Reminder, until the testsuite constructor is fully converted we are going to
          // have a error on this line.
          // @TODO: cleanup this comment after TestSuite conversion.
          $test = new DataProvider($className.'::'.$name);

          $groups = $test->getGroupsFromAnnotation();

          if ($data instanceof TestInterface) {
            $test->addTest($data, $groups);
          } else if (is_array($data)) {
            foreach ($data as $_dataName => $_data) {
              // JEO: At times $_dataName can be a int somehow? Let's coerce it to string.
              $args = Vector {$name, $_data, strval($_dataName)};
              $_test = self::createTest_Simple($theClass, $name, $args);
              $test->addTest($_test, $groups);
            }
          }
        } else {
          $test = self::createTest_Simple($theClass, $name, Vector {$name});
        }
      }
    }

    if (!$test instanceof TestInterface) {
      throw new PHPUnitException('No valid test provided.');
    }

    $test->setName($name);

    return $test;

  }

}
