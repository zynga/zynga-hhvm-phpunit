<?hh // strict

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace SebastianBergmann\PHPUnit;

use SebastianBergmann\PHPUnit\Constraints\Factory as ConstraintFactory;
use SebastianBergmann\PHPUnit\Constraints\ArrayHasKeyConstraint;
use SebastianBergmann\PHPUnit\Constraints\ArraySubsetConstraint;
use SebastianBergmann\PHPUnit\Constraints\StringContainsConstraint;
use SebastianBergmann\PHPUnit\Constraints\TraversableContainsConstraint;
use SebastianBergmann\PHPUnit\Constraints\NotConstraint;
use SebastianBergmann\PHPUnit\Exceptions\AssertionFailedException;
use SebastianBergmann\PHPUnit\Exceptions\AttributeNotFoundException;
use SebastianBergmann\PHPUnit\Exceptions\ExpectationFailedException;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentExceptionFactory;
use SebastianBergmann\PHPUnit\Interfaces\ConstraintInterface;
use Zynga\Framework\ReflectionCache\V1\ReflectionClasses;

use \ArrayAccess;
use \Exception;
use \ReflectionClass;
use \ReflectionException;
use \ReflectionObject;
use \ReflectionProperty;

// --
// JEO: So we broke up the assertions into individual static classes to make
// them more readable and to keep the 'outward' api interface as light as possible.
//
// As the autoloader is lazy, it lowers the total amount of code loaded in most
// use cases.
// --
use SebastianBergmann\PHPUnit\Assertions\Count as AssertionsCount;
use SebastianBergmann\PHPUnit\Assertions\AssertArrayHasKey;
use SebastianBergmann\PHPUnit\Assertions\AssertArrayNotHasKey;
use SebastianBergmann\PHPUnit\Assertions\AssertArraySubset;
use SebastianBergmann\PHPUnit\Assertions\GetObjectAttribute;

class Assertions {

  private static ?AssertionsCount $count;

  final public function count(): AssertionsCount {
    if (!self::$count instanceof AssertionsCount) {
      self::$count = new AssertionsCount();
    }
    return self::$count;
  }

  /**
   * Asserts that an array has a specified key.
   *
   * @param mixed             $key
   * @param array|ArrayAccess $array
   * @param string            $message
   *
   * @since Method available since Release 3.0.0
   */
  final public function assertArrayHasKey(
    mixed $key,
    mixed $array,
    string $message = '',
  ): bool {

    return AssertArrayHasKey::evaluate($this, $key, $array, $message);

  }

  /**
   * Asserts that an array has a specified subset.
   *
   * @param array|ArrayAccess $subset
   * @param array|ArrayAccess $array
   * @param bool              $strict  Check for object identity
   * @param string            $message
   *
   * @since Method available since Release 4.4.0
   */
  final public function assertArraySubset(
    mixed $subset,
    mixed $array,
    bool $strict = false,
    string $message = '',
  ): bool {

    return
      AssertArraySubset::evaluate($this, $subset, $array, $strict, $message);

  }

  /**
   * Asserts that an array does not have a specified key.
   *
   * @param mixed             $key
   * @param array|ArrayAccess $array
   * @param string            $message
   *
   * @since Method available since Release 3.0.0
   */
  final public function assertArrayNotHasKey(
    mixed $key,
    mixed $array,
    string $message = '',
  ): bool {

    return AssertArrayNotHasKey::evaluate($this, $key, $array, $message);

  }

  /**
   * Asserts that a haystack contains a needle.
   *
   * @param mixed  $needle
   * @param mixed  $haystack
   * @param string $message
   * @param bool   $ignoreCase
   * @param bool   $checkForObjectIdentity
   * @param bool   $checkForNonObjectIdentity
   *
   * @since Method available since Release 2.1.0
   */
  public function assertContains(
    mixed $needle,
    mixed $haystack,
    string $message = '',
    bool $ignoreCase = false,
    bool $checkForObjectIdentity = true,
    bool $checkForNonObjectIdentity = false,
  ): bool {

    if (is_array($haystack) ||
        $haystack instanceof Traversable ||
        $haystack instanceof Vector ||
        $haystack instanceof Map) {

      $constraint = ConstraintFactory::factory('TraversableContains');
      $constraint->setExpected($needle);

      if ($constraint instanceof TraversableContainsConstraint) {
        $constraint->setCheckForObjectIdentity($checkForObjectIdentity);
        $constraint->setCheckForNonObjectIdentity($checkForNonObjectIdentity);
      }

      return $this->assertThat($haystack, $constraint, $message);

    } else if (is_string($haystack)) {

      if (!is_string($needle)) {
        throw InvalidArgumentExceptionFactory::factory(1, 'string');
      }

      $constraint = ConstraintFactory::factory('StringContains');
      $constraint->setExpected($needle);

      if ($constraint instanceof StringContainsConstraint) {
        $constraint->setIgnoreCase($ignoreCase);
      }

      return $this->assertThat($haystack, $constraint, $message);

    }

    throw InvalidArgumentExceptionFactory::factory(
      2,
      'array, traversable or string',
    );

  }

  /**
   * Asserts that a haystack that is stored in a static attribute of a class
   * or an attribute of an object contains a needle.
   *
   * @param mixed         $needle
   * @param string        $haystackAttributeName
   * @param string|object $haystackClassOrObject
   * @param string        $message
   * @param bool          $ignoreCase
   * @param bool          $checkForObjectIdentity
   * @param bool          $checkForNonObjectIdentity
   *
   * @since Method available since Release 3.0.0
   */
  public function assertAttributeContains(
    mixed $needle,
    string $haystackAttributeName,
    mixed $haystackClassOrObject,
    string $message = '',
    bool $ignoreCase = false,
    bool $checkForObjectIdentity = true,
    bool $checkForNonObjectIdentity = false,
  ): bool {

    $haystack =
      $this->readAttribute($haystackClassOrObject, $haystackAttributeName);

    return $this->assertContains(
      $needle,
      $haystack,
      $message,
      $ignoreCase,
      $checkForObjectIdentity,
      $checkForNonObjectIdentity,
    );

  }

  /**
   * Asserts that a haystack does not contain a needle.
   *
   * @param mixed  $needle
   * @param mixed  $haystack
   * @param string $message
   * @param bool   $ignoreCase
   * @param bool   $checkForObjectIdentity
   * @param bool   $checkForNonObjectIdentity
   *
   * @since Method available since Release 2.1.0
   */
  public function assertNotContains(
    mixed $needle,
    mixed $haystack,
    string $message = '',
    bool $ignoreCase = false,
    bool $checkForObjectIdentity = true,
    bool $checkForNonObjectIdentity = false,
  ): bool {

    if (is_array($haystack) ||
        $haystack instanceof Traversable ||
        $haystack instanceof Vector ||
        $haystack instanceof Map) {

      $traversableConstraint =
        ConstraintFactory::factory('TraversableContains');
      $traversableConstraint->setExpected($needle);

      if ($traversableConstraint instanceof TraversableContainsConstraint) {
        $traversableConstraint->setCheckForObjectIdentity(
          $checkForObjectIdentity,
        );
        $traversableConstraint->setCheckForNonObjectIdentity(
          $checkForNonObjectIdentity,
        );
      }

      $notConstraint = ConstraintFactory::factory('Not');
      $notConstraint->setExpected($traversableConstraint);

      return $this->assertThat($haystack, $notConstraint, $message);

    } else if (is_string($haystack)) {

      if (!is_string($needle)) {
        throw InvalidArgumentExceptionFactory::factory(1, 'string');
      }

      $stringContainsConstraint =
        ConstraintFactory::factory('StringContains');

      $stringContainsConstraint->setExpected($needle);

      if ($stringContainsConstraint instanceof StringContainsConstraint) {
        $stringContainsConstraint->setIgnoreCase($ignoreCase);
      }

      $notConstraint = ConstraintFactory::factory('Not');
      $notConstraint->setExpected($stringContainsConstraint);

      return $this->assertThat($haystack, $notConstraint, $message);

    }

    throw InvalidArgumentExceptionFactory::factory(
      2,
      'array, traversable or string',
    );

  }

  /**
   * Evaluates a PHPUnit_Framework_Constraint matcher object.
   *
   * @param mixed                        $value
   * @param PHPUnit_Framework_Constraint $constraint
   * @param string                       $message
   *
   * @since Method available since Release 3.0.0
   */
  public function assertThat(
    mixed $value,
    ConstraintInterface $constraint,
    string $message = '',
  ): bool {

    $this->count()->increment($constraint->count());

    try {
      return $constraint->evaluate($value, $message, true);
    } catch (ExpectationFailedException $e) {
      throw new AssertionFailedException($e->getMessage(), $e->getCode(), $e);
    }

  }

  /**
   * Returns a PHPUnit_Framework_Constraint_Not matcher object.
   *
   * @param PHPUnit_Framework_Constraint $constraint
   *
   * @return PHPUnit_Framework_Constraint_Not
   *
   * @since Method available since Release 3.0.0
   */
  //public function logicalNot(
  //  ConstraintInterface $constraint,
  //): ConstraintInterface {
  //  return new NotConstraint($constraint);
  //}

  /**
   * Asserts that a condition is true.
   *
   * @param bool   $condition
   * @param string $message
   *
   * @throws AssertionFailedException
   */
  public function assertTrue(bool $condition, string $message = ''): bool {
    $constraint = ConstraintFactory::factory('IsTrue');
    return $this->assertThat($condition, $constraint, $message);
  }

  /**
   * Fails a test with the given message.
   *
   * @param string $message
   *
   * @throws PHPUnit_Framework_AssertionFailedError
   */
  public function fail(string $message = ''): void {
    throw new AssertionFailedException($message);
  }

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
  public function readAttribute(
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

      return $this->getStaticAttribute($classOrObject, $attributeName);

    } else if (is_object($classOrObject)) {

      return $this->getObjectAttribute($classOrObject, $attributeName);

    } else {
      throw InvalidArgumentExceptionFactory::factory(
        1,
        'class name or object',
      );
    }
  }

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
  public function getStaticAttribute(
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

      $value = $this->_getStaticAttributeFromClass($class, $attributeName);

      if ($value != '[JEO-ATTRIBUTE_NOT_FOUND]') {
        return $value;
      }

    }

    throw new AttributeNotFoundException(
      sprintf('Attribute "%s" not found in class.', $attributeName),
    );

  }

  private function _getStaticAttributeFromClass(
    ReflectionClass $class,
    string $attributeName,
  ): mixed {

    $attributes = $class->getStaticProperties();

    if (array_key_exists($attributeName, $attributes)) {
      return $attributes[$attributeName];
    }

    $parentClass = $class->getParentClass();

    if ($parentClass instanceof ReflectionClass) {
      return
        $this->_getStaticAttributeFromClass($parentClass, $attributeName);
    }

    return '[JEO-ATTRIBUTE_NOT_FOUND]';

  }

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
  public function getObjectAttribute(
    mixed $object,
    string $attributeName,
  ): mixed {

    return GetObjectAttribute::evaluate($object, $attributeName);

  }

}
