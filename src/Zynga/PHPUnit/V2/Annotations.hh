<?hh // strict

namespace Zynga\PHPUnit\V2;

use Zynga\Framework\ReflectionCache\V1\ReflectionClasses;
use Zynga\Framework\ReflectionCache\V1\ReflectionMethods;
use Zynga\Framework\ReflectionCache\V1\ReflectionClassDocComments;
use Zynga\Framework\ReflectionCache\V1\ReflectionMethodDocComments;
use Zynga\PHPUnit\V2\Annotations\AnnotationsContainer;

use \ReflectionClass;
use \ReflectionException;
use \ReflectionMethod;

class Annotations {

  private static Map<string, AnnotationsContainer> $_classCache = Map {};
  private static Map<string, AnnotationsContainer> $_methodCache = Map {};

  public static function createAnnotationContainerFromAttributeData(
    mixed $rawData,
  ): AnnotationsContainer {

    $data = new AnnotationsContainer();

    if (is_array($rawData) && count($rawData) > 0) {

      $data = new AnnotationsContainer();
      $data->addValuesFromAttributes($rawData);

    }

    return $data;

  }

  public static function parseClassAnnotations(
    string $className,
  ): AnnotationsContainer {

    $key = $className;

    $data = self::$_classCache->get($key);

    if ($data instanceof AnnotationsContainer) {
      return $data;
    }

    // Find the class attribute data and pull it across into our stack.
    $class = ReflectionClasses::getReflection($className);

    if ($class instanceof ReflectionClass) {

      $rawData = $class->getAttributes();

      $data = self::createAnnotationContainerFromAttributeData($rawData);

      self::$_classCache->set($key, $data);

      return $data;

    }

    $data = new AnnotationsContainer();

    self::$_classCache->set($key, $data);

    return $data;

  }

  public static function parseMethodAnnotations(
    string $className,
    string $methodName,
  ): AnnotationsContainer {

    $key = $className.'::'.$methodName;

    $data = self::$_methodCache->get($key);

    if ($data instanceof AnnotationsContainer) {
      return $data;
    }

    // Find the method attribute data and pull it across into our stack.
    $method = ReflectionMethods::getReflection($className, $methodName);

    if ($method instanceof ReflectionMethod) {

      $rawData = $method->getAttributes();

      $data = self::createAnnotationContainerFromAttributeData($rawData);

      self::$_methodCache->set($key, $data);

      return $data;

    }

    // --
    // Default behavior is there is no annotations on this item, just return as, we are now done.
    // --
    $data = new AnnotationsContainer();
    self::$_methodCache->set($key, $data);
    return $data;

  }

  /**
   * @param string $className
   * @param string $methodName
   *
   * @return array
   *
   * @throws ReflectionException
   *
   * @since Method available since Release 3.4.0
   */
  public static function parseTestMethodAnnotations(
    string $className,
    string $methodName = '',
  ): Map<string, Map<string, Vector<string>>> {

    $classValues = self::parseClassAnnotations($className)->getValuesAsMap();

    if ($methodName == '') {
      return Map {'class' => $classValues, 'method' => Map {}};
    }

    $methodValues =
      self::parseMethodAnnotations($className, $methodName)->getValuesAsMap();

    return Map {'class' => $classValues, 'method' => $methodValues};

  }

  final public static function getAllAnnotationsForKey(
    string $key,
    string $className,
    string $methodName = '',
  ): Vector<string> {

    $collapsed = Vector {};

    $classAnnotations = self::parseClassAnnotations($className);

    $classValues = $classAnnotations->getValuesForKey($key);

    $collapsed->addAll($classValues);

    // var_dump($key);
    // var_dump($className);
    // var_dump($classAnnotations);
    // var_dump($classValues);
    // var_dump($collapsed);

    $methodAnnotations =
      self::parseMethodAnnotations($className, $methodName);

    $methodValues = $methodAnnotations->getValuesForKey($key);

    // var_dump($key);
    // var_dump($className);
    // var_dump($methodAnnotations);
    // var_dump($methodValues);
    // var_dump($collapsed);

    $collapsed->addAll($methodValues);

    // return the stack of annotation combined
    return $collapsed;

  }

  final public static function getAnnotationsForKey(
    string $context,
    string $key,
    string $className,
    string $methodName = '',
  ): Vector<string> {

    $allAnnotations =
      self::parseTestMethodAnnotations($className, $methodName);

    $contextAnnotations = $allAnnotations->get($context);

    if ($contextAnnotations instanceof Map) {

      $keyValue = $contextAnnotations->get($key);

      if ($keyValue instanceof Vector) {
        return $keyValue;
      }

    }

    // not found
    return Vector {};

  }

}
