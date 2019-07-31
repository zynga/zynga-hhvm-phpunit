<?hh // strict

namespace Zynga\PHPUnit\V2\TestSuite;

use Zynga\PHPUnit\V2\Exceptions\Exception as PHPUnitException;
use Zynga\PHPUnit\V2\Exceptions\TestError\IncompleteException;
use Zynga\PHPUnit\V2\Exceptions\TestError\SkippedException;
use
  Zynga\PHPUnit\V2\Exceptions\TestSuite\DataProvider\InvalidDataProviderMethodException
;
use Zynga\PHPUnit\V2\Annotations;
use Zynga\PHPUnit\V2\IncompleteTestCase;
use Zynga\PHPUnit\V2\Interfaces\TestInterface;
use Zynga\PHPUnit\V2\SkippedTestCase;
use Zynga\PHPUnit\V2\TestSuite\DataProvider;
use Zynga\PHPUnit\V2\TestSuite\DataProvider\Loader;
use Zynga\PHPUnit\V2\WarningTestCase;

use \Exception;
use \ReflectionClass;
use \ReflectionMethod;
use \ReflectionType;

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

      return $data;

    } catch (InvalidDataProviderMethodException $e) {

      $message = sprintf(
        'Test for %s::%s marked incomplete, dataprovider was invalid: ',
        $className,
        $name,
      );

      $_message = $e->getMessage();

      if ($_message != '') {
        $message .= "\n".$_message;
      }

      $data = self::incompleteTest($className, $name, $message);
      return $data;

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
      return $data;

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
      return $data;

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
      return $data;
    }

    // Default unhandled situatono, where the $t = null, and exception trapping didn't happen.
    return null;

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

      $paramCount = count($parameters);

      if ($paramCount < 2) {
        $test = self::createTest_Simple($theClass, $name, Vector {$name});
      } else {

        // Support for dataprovider based loading.
        // TestCase($name, $data)
        $data = self::createTest_LoadData($theClass, $name);

        // Test method with @dataProvider.
        if ($data != null) {

          $test = self::createViaProvidedData($theClass, $name, $data);

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

  private static function createViaProvidedData(
    ReflectionClass $theClass,
    string $name,
    mixed $data,
  ): TestInterface {

    $className = $theClass->getName();

    $test = new DataProvider($className.'::'.$name);

    // Handle the use case of a provider adding a testinterface on the fly.
    if ($data instanceof TestInterface) {
      return self::addToDataProvidedTest_TestInterface(
        $theClass,
        $name,
        $test,
        $data,
      );
    }

    // Handle the use case of a object based param
    if (is_object($data)) {
      return self::addToDataProvidedTest_ObjectParam(
        $theClass,
        $name,
        $test,
        $data,
      );
    }

    // Handle the use case of a array based param
    if (is_array($data)) {
      return self::addToDataProvidedTest_ArrayParam(
        $theClass,
        $name,
        $test,
        $data,
      );
    }

    return $test;

  }

  private static function addToDataProvidedTest_TestInterface(
    ReflectionClass $theClass,
    string $name,
    DataProvider $test,
    TestInterface $data,
  ): TestInterface {
    $groups = $test->getGroupsFromAnnotation();
    $test->addTest($data, $groups);
    return $test;
  }

  private static function addToDataProvidedTest_ObjectParam(
    ReflectionClass $theClass,
    string $name,
    DataProvider $test,
    mixed $data,
  ): TestInterface {

    $className = $theClass->getName();
    $groups = $test->getGroupsFromAnnotation();

    if (self::verifyTestArgumentsFitShapeRequested($theClass, $name, $data) ===
        false) {

      $incompleteTest = new IncompleteTestCase(
        $theClass->getName(),
        $name,
        'Data was invalid for test data='.
        get_class($data).
        ' className='.
        $className.
        ' method='.
        $name,
      );

      $test->addTest($incompleteTest, $groups);

      return $test;

    }

    $args = Vector {
      $name,
      array($data),
      Loader::getProviderFunctionName($className, $name),
    };

    $_test = self::createTest_Simple($theClass, $name, $args);

    $test->addTest($_test, $groups);

    return $test;

  }

  private static function addToDataProvidedTest_ArrayParam(
    ReflectionClass $theClass,
    string $name,
    DataProvider $test,
    mixed $data,
  ): TestInterface {

    // --
    // Verify that the shape of the params being passed in are kosher with the
    // shape of the receiving function.
    // --
    $groups = $test->getGroupsFromAnnotation();
    $className = $theClass->getName();

    if (is_array($data)) {

      foreach ($data as $_dataName => $_data) {

        if (self::verifyTestArgumentsFitShapeRequested(
              $theClass,
              $name,
              $_data,
            ) ===
            true) {

          // JEO: At times $_dataName can be a int somehow? Let's coerce it to string.
          $args = Vector {$name, $_data, strval($_dataName)};
          $_test = self::createTest_Simple($theClass, $name, $args);
          $test->addTest($_test, $groups);

        } else {

          $incompleteTest = new IncompleteTestCase(
            $theClass->getName(),
            $name,
            'Data was invalid for test data='.
            json_encode($_data).
            ' className='.
            $className.
            ' method='.
            $name,
          );

          $test->addTest($incompleteTest, $groups);

        }

      }

    }

    return $test;
  }

  private static function verifyTestArgumentsFitShapeRequested(
    ReflectionClass $theClass,
    string $name,
    mixed $args,
  ): bool {

    // Extract the method for the test, it should exist and be  static.
    $theTestMethod = $theClass->getMethod($name);

    if (!$theTestMethod instanceof ReflectionMethod) {
      return false;
    }

    $t_args = array();

    if (is_object($args)) {
      $t_args[] = $args;
    } else if (is_array($args)) {
      $t_args = $args;
    }

    // Test #1) Check that we have the correct number of arguments for the function.
    $requiredParamCount = $theTestMethod->getNumberOfRequiredParameters();
    $paramCount = count($t_args);

    if ($requiredParamCount > 0 && $paramCount < $requiredParamCount) {
      return false;
    }

    // Test #2) Check that the params are of the same shape as the args provided
    $params = $theTestMethod->getParameters();

    $offset = 0;
    foreach ($params as $param) {
      $paramType = $param->getType();
      $arg = $t_args[$offset];
      if ($paramType instanceof ReflectionType &&
          self::verifyParamVsValueProvided($paramType, $arg)) {
        // We are gtg, noop.
        $offset++;
        continue;
      } else {
        return false;
      }
    }

    return true;

  }

  private static function verifyParamVsValueProvided(
    ReflectionType $param,
    mixed $value,
  ): bool {

    $paramType = $param->__toString();
    $argType = gettype($value);

    if ($argType == 'object') {
      $argType = get_class($value);
      if ($argType == $paramType) {
        return true;
      }
    } else {
      // It is some form of internal type.
      if ($paramType == "HH\\int" && $argType == 'integer') {
        return true;
      } else if ($paramType == "HH\\string" && $argType == 'string') {
        return true;
      } else if ($paramType == "HH\\float" &&
                 ($argType == 'float' || $argType == 'double')) {
        return true;
      }

    }

    return false;

  }

}
