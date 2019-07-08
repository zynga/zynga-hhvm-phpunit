<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use SebastianBergmann\PHPUnit\Constraints\ClassHasAttributeConstraint;
use SebastianBergmann\PHPUnit\Constraints\NotConstraint;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentExceptionFactory;

class AssertClassNotHasAttribute {

  /**
   * Asserts that a class does not have a specified attribute.
   *
   * @param string $attributeName
   * @param string $className
   * @param string $message
   *
   * @since Method available since Release 3.1.0
   */
  public static function evaluate(
    Assertions $assertions,
    string $attributeName,
    string $className,
    string $message = '',
  ): bool {

    if (!is_string($attributeName)) {
      throw InvalidArgumentExceptionFactory::factory(1, 'string');
    }

    if (!preg_match(
          '/[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*/',
          $attributeName,
        )) {
      throw InvalidArgumentExceptionFactory::factory(
        1,
        'valid attribute name',
      );
    }

    if (!is_string($className) || !class_exists($className)) {
      throw InvalidArgumentExceptionFactory::factory(
        2,
        'class name',
        $className,
      );
    }

    $classHasAttribute = new ClassHasAttributeConstraint();
    $classHasAttribute->setExpected($attributeName);

    $notConstraint = new NotConstraint();
    $notConstraint->setExpected($classHasAttribute);

    return $assertions->assertThat($className, $notConstraint, $message);

  }

}
