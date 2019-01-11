<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Exceptions\AttributeNotFoundException;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentExceptionFactory;

use \ReflectionClass;
use \ReflectionObject;
use \ReflectionProperty;
use \Exception;

class GetObjectAttribute {

  /**
   * Returns the value of an object's attribute.
   * This also works for attributes that are declared protected or private.
   *
   * @param object $object
   * @param string $attributeName
   *
   * @return mixed
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since Method available since Release 4.0.0
   */
  final public static function evaluate(
    mixed $object,
    string $attributeName,
  ): mixed {

    if (!is_object($object)) {
      throw InvalidArgumentExceptionFactory::factory(1, 'object');
    }

    if (!is_string($attributeName)) {
      throw InvalidArgumentExceptionFactory::factory(2, 'string');
    }

    if (!preg_match(
          '/[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*/',
          $attributeName,
        )) {

      throw InvalidArgumentExceptionFactory::factory(
        2,
        'valid attribute name',
      );

    }

    $reflectionObject = new ReflectionObject($object);

    $attribute =
      self::_getReflectionProperty($reflectionObject, $attributeName);

    if ($attribute instanceof ReflectionProperty) {

      $attribute->setAccessible(true);
      $value = $attribute->getValue($object);

      return $value;

    }

    throw new AttributeNotFoundException(
      sprintf('Attribute "%s" not found in object.', $attributeName),
    );

  }

  final private static function _getReflectionProperty(
    ReflectionClass $reflector,
    string $attributeName,
  ): ?ReflectionProperty {

    try {
      $attribute = $reflector->getProperty($attributeName);
      return $attribute;
    } catch (Exception $e) {
      $parentClass = $reflector->getParentClass();
      if ($parentClass instanceof ReflectionClass) {
        return self::_getReflectionProperty($parentClass, $attributeName);
      }
    }

    return null;

  }
}
