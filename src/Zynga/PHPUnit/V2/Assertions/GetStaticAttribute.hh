<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use SebastianBergmann\PHPUnit\Exceptions\AttributeNotFoundException;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentExceptionFactory;
use Zynga\Framework\ReflectionCache\V1\ReflectionClasses;
use \ReflectionClass;

class GetStaticAttribute {

  /**
   * Returns the value of a static attribute.
   * This also works for attributes that are declared protected or private.
   *
   * @param string $className
   * @param string $attributeName
   *
   * @return mixed
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since Method available since Release 4.0.0
   */
  final public static function evaluate(
    string $className,
    string $attributeName,
  ): mixed {

    if (!is_string($className)) {
      throw InvalidArgumentExceptionFactory::factory(1, 'string');
    }

    if (!class_exists($className)) {
      throw InvalidArgumentExceptionFactory::factory(1, 'class name');
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

    $class = ReflectionClasses::getReflection($className);

    if ($class instanceof ReflectionClass) {

      $value = self::_getStaticAttributeFromClass($class, $attributeName);

      if ($value != '[JEO-ATTRIBUTE_NOT_FOUND]') {
        return $value;
      }

    }

    throw new AttributeNotFoundException(
      sprintf('Attribute "%s" not found in class.', $attributeName),
    );

  }

  final private static function _getStaticAttributeFromClass(
    ReflectionClass $class,
    string $attributeName,
  ): mixed {

    $attributes = $class->getStaticProperties();

    if (array_key_exists($attributeName, $attributes)) {
      return $attributes[$attributeName];
    }

    $parentClass = $class->getParentClass();

    if ($parentClass instanceof ReflectionClass) {
      return self::_getStaticAttributeFromClass($parentClass, $attributeName);
    }

    return '[JEO-ATTRIBUTE_NOT_FOUND]';

  }

}
