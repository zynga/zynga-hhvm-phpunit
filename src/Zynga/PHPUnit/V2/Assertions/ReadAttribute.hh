<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentExceptionFactory;

class ReadAttribute {

  /**
   * Returns the value of an attribute of a class or an object.
   * This also works for attributes that are declared protected or private.
   *
   * @param string|object $classOrObject
   * @param string        $attributeName
   *
   * @return mixed
   *
   * @throws PHPUnit_Framework_Exception
   */
  final static public function evaluate(
    Assertions $assertions,
    mixed $classOrObject,
    string $attributeName,
  ): mixed {

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

    if (is_string($classOrObject)) {
      if (!class_exists($classOrObject)) {
        throw InvalidArgumentExceptionFactory::factory(1, 'class name');
      }

      return $assertions->getStaticAttribute($classOrObject, $attributeName);

    } else if (is_object($classOrObject)) {

      return $assertions->getObjectAttribute($classOrObject, $attributeName);

    } else {
      throw InvalidArgumentExceptionFactory::factory(
        1,
        'class name or object',
      );
    }
  }

}
