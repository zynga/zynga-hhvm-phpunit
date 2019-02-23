<?hh // strict

namespace Zynga\PHPUnit\V2;

use Zynga\Framework\ReflectionCache\V1\ReflectionClassDocComments;
use Zynga\Framework\ReflectionCache\V1\ReflectionMethodDocComments;

use \ReflectionClass;
use \ReflectionException;
use \ReflectionMethod;

class Annotations {

  private static Map<string, Map<string, Vector<string>>>
    $_classCache = Map {};
  private static Map<string, Map<string, Vector<string>>>
    $_methodCache = Map {};

  /**
   * @param string $docblock
   *
   * @return array
   *
   * @since Method available since Release 3.4.0
   */
  private static function parseAnnotations(
    string $docblock,
  ): Map<string, Vector<string>> {

    $annotations = Map {};

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

        $currentValue = $annotations->get($key);

        if ($currentValue == null) {
          $currentValue = Vector {};
          $currentValue->add($value);
        } else {
          $currentValue->add($value);
        }
        $annotations->set($key, $currentValue);
      }

    }

    return $annotations;

  }

  public static function parseClassAnnotations(
    string $className,
  ): Map<string, Vector<string>> {

    $key = $className;

    $data = self::$_classCache->get($key);

    if ($data instanceof Map) {
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
  ): Map<string, Vector<string>> {

    $key = $className.'::'.$methodName;

    $data = self::$_methodCache->get($key);

    if ($data instanceof Map) {
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

    if ($methodName == '') {
      return Map {
        'class' => self::parseClassAnnotations($className),
        'method' => Map {},
      };
    }

    return Map {
      'class' => self::parseClassAnnotations($className),
      'method' => self::parseMethodAnnotations($className, $methodName),
    };

  }

}
