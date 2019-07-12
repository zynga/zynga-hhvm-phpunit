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

    $dataProviderMethods = Annotations::getAllAnnotationsForKey(
      'dataProvider',
      $className,
      $methodName,
    );

    $data = null;

    foreach ($dataProviderMethods as $dataProviderMethod) {

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

}
