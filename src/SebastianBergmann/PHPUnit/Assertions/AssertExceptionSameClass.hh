<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;
use Zynga\Framework\ReflectionCache\V1\ReflectionClasses;
use \ReflectionClass;
use \Exception;

class AssertExceptionSameClass {

  public static function evaluate(
    Assertions $assertions,
    mixed $expectedValue,
    mixed $actualValue,
    string $message,
  ): bool {

    $expectedClassName = self::resolveObjectToClassName($expectedValue);
    $actualClassName = self::resolveObjectToClassName($actualValue);

    return $assertions->assertEquals(
      $expectedClassName,
      $actualClassName,
      $message,
    );

  }

  private static function resolveObjectToClassName(mixed $object): string {

    if (is_string($object)) {
      return $object;
    }

    if (!is_object($object)) {
      return '';
    }

    $reflected = ReflectionClasses::getReflection($object);

    if ($reflected instanceof ReflectionClass) {

      $className = '';

      if ($reflected->inNamespace() == true) {

        $className .= $reflected->getNamespaceName().'\\';

      }

      $className .= $reflected->getName();

      return $className;

    }

    return '';

  }

}
