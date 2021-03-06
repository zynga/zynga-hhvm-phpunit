<?hh // strict

namespace Zynga\PHPUnit\V2\TestSuite\DataProvider;

use Zynga\PHPUnit\V2\Exceptions\Exception as PHPUnitException;

use Zynga\Framework\ReflectionCache\V1\ReflectionClasses;
//use Zynga\Framework\ReflectionCache\V1\ReflectionFunctions;
use Zynga\Framework\ReflectionCache\V1\ReflectionMethods;
// use Zynga\PHPUnit\V2\Interfaces\TestInterface;
// use Zynga\PHPUnit\V2\TestCase;

use \Exception;
use \ReflectionClass;
use \ReflectionMethod;

use Zynga\PHPUnit\V2\Annotations;
use Zynga\Framework\Dynamic\V1\DynamicMethodCall;
use
  Zynga\PHPUnit\V2\Exceptions\TestSuite\DataProvider\InvalidDataProviderMethodException
;

class Loader {

  /**
   * Returns the provided data for a method.
   *
   * @param string $className
   * @param string $methodName
   *
   * @return array|Iterator when a data provider is specified and exists
   *                        null           when no data provider is specified
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since Method available since Release 3.2.0
   */
  public static function getProvidedData(
    string $className,
    string $methodName,
  ): mixed {

    $data = null;

    $dataProviderMethod =
      self::getProviderFunctionName($className, $methodName);

    if (is_string($dataProviderMethod)) {

      $reflectionMethod =
        ReflectionMethods::getReflection($className, $dataProviderMethod);

      if (!$reflectionMethod instanceof ReflectionMethod) {

        throw new InvalidDataProviderMethodException(
          'method='.$dataProviderMethod.' does not exist',
        );
        
      }

      if ($reflectionMethod->isStatic() === false) {

        throw new InvalidDataProviderMethodException(
          'method='.$dataProviderMethod.' is static',
        );

      }

      $data = DynamicMethodCall::callMethod(
        $className,
        $dataProviderMethod,
        Vector {},
      );

      if (is_array($data)) {
        return $data;
      }

    }

    return $data;

  }

  public static function getProviderFunctionName(
    string $className,
    string $methodName,
  ): ?string {

    $dataProviderMethods = Annotations::getAllAnnotationsForKey(
      'dataProvider',
      $className,
      $methodName,
    );

    foreach ($dataProviderMethods as $dataProviderMethod) {
      return $dataProviderMethod;
    }

    return null;

  }

}
