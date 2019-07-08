<?hh // strict

namespace Zynga\PHPUnit\V2;

use Zynga\Framework\ReflectionCache\V1\ReflectionClassDocComments;
use Zynga\Framework\ReflectionCache\V1\ReflectionMethodDocComments;
use Zynga\PHPUnit\V2\Annotations\AnnotationsContainer;

use \ReflectionClass;
use \ReflectionException;
use \ReflectionMethod;

class Annotations {

  private static Map<string, AnnotationsContainer> $_classCache = Map {};
  private static Map<string, AnnotationsContainer> $_methodCache = Map {};

  /**
   * @param string $docblock
   *
   * @return array
   *
   * @since Method available since Release 3.4.0
   */
  private static function parseAnnotations(
    string $docblock,
  ): AnnotationsContainer {

    $annotations = new AnnotationsContainer();

    // Strip away the docblock header and footer to ease parsing of one line annotations
    $docblock = substr($docblock, 3, -2);

    $matches = array();

    if (preg_match_all(
          '/@(?P<name>[A-Za-z_-]+)(?:[ \t]+(?P<value>.*?))?[ \t]*\r?$/m',
          $docblock,
          $matches,
        )) {
      $numMatches = count($matches[0]);

      for ($i = 0; $i < $numMatches; ++$i) {

        $key = $matches['name'][$i];
        $value = $matches['value'][$i];

        $annotations->addValueToKey($key, $value);

      }

    }

    return $annotations;

  }

  public static function parseClassAnnotations(
    string $className,
  ): AnnotationsContainer {

    $key = $className;

    $data = self::$_classCache->get($key);

    if ($data instanceof AnnotationsContainer) {
      return $data;
    }

    $data = Map {};

    $docComment = ReflectionClassDocComments::getDocComment($className);

    $data = self::parseAnnotations(strval($docComment));

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

    $data = Map {};

    $docComment =
      ReflectionMethodDocComments::getDocComment($className, $methodName);

    $data = self::parseAnnotations(strval($docComment));

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

    $classValues =
      self::getAnnotationsForKey('class', $key, $className, $methodName);

    foreach ($classValues as $classValue) {
      if (is_string($classValue)) {
        $collapsed->add($classValue);
      }
    }

    $methodValues =
      self::getAnnotationsForKey('method', $key, $className, $methodName);

    foreach ($methodValues as $methodValue) {
      if (is_string($methodValue)) {
        $collapsed->add($methodValue);
      }
    }

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
