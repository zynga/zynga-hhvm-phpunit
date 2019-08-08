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
use Zynga\Framework\ReflectionCache\V1\ReflectionClasses;

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

    list($argOk, $argError) =
      self::verifyTestArgumentsFitShapeRequested($theClass, $name, $data);

    if ($argOk !== true) {

      $incompleteTest =
        new IncompleteTestCase(
          $className,
          $name,
          sprintf(
            'Data was invalid for test  className=%s method=%s data=%s argError=%s',
            $className,
            $name,
            json_encode($data),
            $argError,
          ),
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

  private static function isAssociativeArray(mixed $data): bool {
    if (!is_array($data)) {
      return false;
    }
    if (array() === $data) {
      return false;
    }
    return array_keys($data) !== range(0, count($data) - 1);
  }

  private static function addToDataProvidedTest_ArrayParam(
    ReflectionClass $theClass,
    string $name,
    DataProvider $test,
    mixed $data,
  ): TestInterface {

    if (!is_array($data)) {
      return $test;
    }

    // --
    // Verify that the shape of the params being passed in are kosher with the
    // shape of the receiving function.
    // --
    $groups = $test->getGroupsFromAnnotation();
    $className = $theClass->getName();

    // First is this an associative array for $data

    $isAssoc = self::isAssociativeArray($data);

    if ($isAssoc === true) {

      foreach ($data as $_dataKey => $_dataValues) {

        if (is_array($_dataValues)) {

          foreach ($_dataValues as $_dataValue) {

            $_data = array($_dataKey, $_dataValue);
            $_dataName = $_dataKey.' '.json_encode($_dataValue);

            list($argOk, $argError) =
              self::verifyTestArgumentsFitShapeRequested(
                $theClass,
                $name,
                $_data,
              );

            if ($argOk === true) {

              $args = Vector {$name, $_data, $_dataName};
              $_test = self::createTest_Simple($theClass, $name, $args);
              $test->addTest($_test, $groups);

            } else {

              $incompleteTest =
                new IncompleteTestCase(
                  $className,
                  $name,
                  sprintf(
                    'Data was invalid for test  className=%s method=%s data=%s argError=%s',
                    $className,
                    $name,
                    json_encode($_data),
                    $argError,
                  ),
                );

              $test->addTest($incompleteTest, $groups);
            }

          }

        }

      }

    } else {

      foreach ($data as $_dataName => $_data) {

        list($argOk, $argError) = self::verifyTestArgumentsFitShapeRequested(
          $theClass,
          $name,
          $_data,
        );

        if ($argOk === true) {

          // JEO: At times $_dataName can be a int somehow? Let's coerce it to string.
          $args = Vector {$name, $_data, strval($_dataName)};
          $_test = self::createTest_Simple($theClass, $name, $args);
          $test->addTest($_test, $groups);

        } else {

          $incompleteTest =
            new IncompleteTestCase(
              $className,
              $name,
              sprintf(
                'Data was invalid for test  className=%s method=%s data=%s argError=%s',
                $className,
                $name,
                json_encode($_data),
                $argError,
              ),
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
  ): (bool, string) {

    // Extract the method for the test, it should exist and be  static.
    $theTestMethod = $theClass->getMethod($name);

    if (!$theTestMethod instanceof ReflectionMethod) {
      return tuple(false, 'unable to reflect method='.$name);
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
      return tuple(
        false,
        'method='.
        $name.
        ' requiredParamCount='.
        $requiredParamCount.
        ' provided='.
        $paramCount,
      );

    }

    // Test #2) Check that the params are of the same shape as the args provided
    $params = $theTestMethod->getParameters();

    $offset = 0;
    foreach ($params as $param) {

      $paramType = $param->getType();

      // --
      // This is an aoptional param, and we didn't provide data for the argument.
      // --
      if ($param->isOptional() && !array_key_exists($offset, $t_args)) {
        continue;
      }

      $arg = $t_args[$offset];

      if ($paramType instanceof ReflectionType) {

        list($paramVerify, $paramError) =
          self::verifyParamVsValueProvided($paramType, $arg);

        if ($paramVerify === true) {
          // We are gtg, noop.
          $offset++;
          continue;
        }

        return tuple(false, 'param value is invalid '.$paramError);

      } else {

        return tuple(
          false,
          'unable to reflect testMethod='.$name.' arg='.$offset,
        );

      }
    }

    return tuple(true, '');

  }

  private static function doesImplementInterface(
    string $class,
    string $desiredInterface,
  ): bool {

    $classReflection = ReflectionClasses::getReflection($class);

    if ($classReflection instanceof ReflectionClass) {

      // does the class itself implement it directly.
      if ($classReflection->implementsInterface($desiredInterface) === true) {
        return true;
      }

      // walk up the tree to see if one of the parents implements the target interface
      $parentClass = $classReflection->getParentClass();

      if ($parentClass instanceof ReflectionClass) {

        return self::doesImplementInterface(
          $parentClass->getName(),
          $desiredInterface,
        );

      }

    }

    // no joy, no class matching found
    return false;

  }
  private static function verifyParamVsValueProvided(
    ReflectionType $param,
    mixed $value,
  ): (bool, string) {

    $paramType = $param->__toString();
    $argType = gettype($value);

    if ($paramType == 'HH\\mixed') {
      // If the param is mixed, then we are gtg with anything coming in.
      return tuple(true, '');
    }

    if ($argType == 'object') {

      $isInterface = false;

      $paramReflection = ReflectionClasses::getReflection($paramType);

      if ($paramReflection instanceof ReflectionClass) {
        if ($paramReflection->isInterface()) {
          $isInterface = true;
        }
      }

      $argType = get_class($value);

      if ($isInterface === true) {

        if (self::doesImplementInterface($argType, $paramType)) {
          return tuple(true, '');
        }

        return tuple(
          false,
          'missing expecterInterface='.$paramType.' providedClass='.$argType,
        );

      }

      if ($argType == $paramType) {
        return tuple(true, '');
      }

      return tuple(
        false,
        'missing expectedClass='.$paramType.' providedClass='.$argType,
      );

    } else {

      // It is some form of internal type.
      if ($paramType == "HH\\int" && $argType == 'integer') {

        return tuple(true, '');

      } else if ($paramType == "HH\\string" && $argType == 'string') {

        return tuple(true, '');

      } else if ($paramType == "HH\\float" &&
                 ($argType == 'float' || $argType == 'double')) {

        return tuple(true, '');

      } else if ($paramType == "HH\\bool" && $argType == 'boolean') {

        return tuple(true, '');

      }

    }

    return tuple(false, 'expected='.$paramType.' argType='.$argType);

  }

}
