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
use SebastianBergmann\PHPUnit\Assertions\AssertAttributeContains;
use SebastianBergmann\PHPUnit\Assertions\AssertAttributeNotContains;
use SebastianBergmann\PHPUnit\Assertions\AssertContains;
use SebastianBergmann\PHPUnit\Assertions\AssertContainsOnly;
use SebastianBergmann\PHPUnit\Assertions\AssertNotContains;
use SebastianBergmann\PHPUnit\Assertions\AssertContainsOnlyInstancesOf;
use SebastianBergmann\PHPUnit\Assertions\AssertAttributeContainsOnly;
use SebastianBergmann\PHPUnit\Assertions\AssertAttributeNotContainsOnly;
use SebastianBergmann\PHPUnit\Assertions\AssertNotContainsOnly;
use SebastianBergmann\PHPUnit\Assertions\AssertThat;
use SebastianBergmann\PHPUnit\Assertions\AssertTrue;
use SebastianBergmann\PHPUnit\Assertions\GetObjectAttribute;
use SebastianBergmann\PHPUnit\Assertions\GetStaticAttribute;
use SebastianBergmann\PHPUnit\Assertions\ReadAttribute;
use SebastianBergmann\PHPUnit\Interfaces\ConstraintInterface;
use SebastianBergmann\PHPUnit\Exceptions\AssertionFailedException;

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
  final public function assertContains(
    mixed $needle,
    mixed $haystack,
    string $message = '',
    bool $ignoreCase = false,
    bool $checkForObjectIdentity = true,
    bool $checkForNonObjectIdentity = false,
  ): bool {

    return AssertContains::evaluate(
      $this,
      $needle,
      $haystack,
      $message,
      $ignoreCase,
      $checkForObjectIdentity,
      $checkForNonObjectIdentity,
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
  final public function assertAttributeContains(
    mixed $needle,
    string $haystackAttributeName,
    mixed $haystackClassOrObject,
    string $message = '',
    bool $ignoreCase = false,
    bool $checkForObjectIdentity = true,
    bool $checkForNonObjectIdentity = false,
  ): bool {

    return AssertAttributeContains::evaluate(
      $this,
      $needle,
      $haystackAttributeName,
      $haystackClassOrObject,
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
  final public function assertNotContains(
    mixed $needle,
    mixed $haystack,
    string $message = '',
    bool $ignoreCase = false,
    bool $checkForObjectIdentity = true,
    bool $checkForNonObjectIdentity = false,
  ): bool {

    return AssertNotContains::evaluate(
      $this,
      $needle,
      $haystack,
      $message,
      $ignoreCase,
      $checkForObjectIdentity,
      $checkForNonObjectIdentity,
    );

  }

  /**
   * Asserts that a haystack that is stored in a static attribute of a class
   * or an attribute of an object does not contain a needle.
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
  final public function assertAttributeNotContains(
    mixed $needle,
    string $haystackAttributeName,
    mixed $haystackClassOrObject,
    string $message = '',
    bool $ignoreCase = false,
    bool $checkForObjectIdentity = true,
    bool $checkForNonObjectIdentity = false,
  ): bool {

    return AssertAttributeNotContains::evaluate(
      $this,
      $needle,
      $haystackAttributeName,
      $haystackClassOrObject,
      $message,
      $ignoreCase,
      $checkForObjectIdentity,
      $checkForNonObjectIdentity,
    );

  }

  /**
   * Asserts that a haystack contains only values of a given type.
   *
   * @param string $type
   * @param mixed  $haystack
   * @param bool   $isNativeType
   * @param string $message
   *
   * @since Method available since Release 3.1.4
   */
  final public function assertContainsOnly(
    string $type,
    mixed $haystack,
    bool $isNativeType = false,
    string $message = '',
  ): bool {

    return AssertContainsOnly::evaluate(
      $this,
      $type,
      $haystack,
      $isNativeType,
      $message,
    );

  }

  /**
   * Asserts that a haystack contains only instances of a given classname
   *
   * @param string            $classname
   * @param array|Traversable $haystack
   * @param string            $message
   */
  final public function assertContainsOnlyInstancesOf(
    string $classname,
    mixed $haystack,
    string $message = '',
  ): bool {

    return AssertContainsOnlyInstancesOf::evaluate(
      $this,
      $classname,
      $haystack,
      $message,
    );

  }

  /**
   * Asserts that a haystack that is stored in a static attribute of a class
   * or an attribute of an object contains only values of a given type.
   *
   * @param string        $type
   * @param string        $haystackAttributeName
   * @param string|object $haystackClassOrObject
   * @param bool          $isNativeType
   * @param string        $message
   *
   * @since Method available since Release 3.1.4
   */
  final public function assertAttributeContainsOnly(
    string $type,
    string $haystackAttributeName,
    mixed $haystackClassOrObject,
    bool $isNativeType = false,
    string $message = '',
  ): bool {

    return AssertAttributeContainsOnly::evaluate(
      $this,
      $type,
      $haystackAttributeName,
      $haystackClassOrObject,
      $isNativeType,
      $message,
    );

  }

  /**
   * Asserts that a haystack does not contain only values of a given type.
   *
   * @param string $type
   * @param mixed  $haystack
   * @param bool   $isNativeType
   * @param string $message
   *
   * @since Method available since Release 3.1.4
   */
  final public function assertNotContainsOnly(
    string $type,
    mixed $haystack,
    bool $isNativeType = false,
    string $message = '',
  ): bool {

    return AssertNotContainsOnly::evaluate(
      $this,
      $type,
      $haystack,
      $isNativeType,
      $message,
    );

  }

  /**
   * Asserts that a haystack that is stored in a static attribute of a class
   * or an attribute of an object does not contain only values of a given
   * type.
   *
   * @param string        $type
   * @param string        $haystackAttributeName
   * @param string|object $haystackClassOrObject
   * @param bool          $isNativeType
   * @param string        $message
   *
   * @since Method available since Release 3.1.4
   */
  final public function assertAttributeNotContainsOnly(
    string $type,
    string $haystackAttributeName,
    mixed $haystackClassOrObject,
    bool $isNativeType = false,
    string $message = '',
  ): bool {

    return AssertAttributeNotContainsOnly::evaluate(
      $this,
      $type,
      $haystackAttributeName,
      $haystackClassOrObject,
      $isNativeType,
      $message,
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
  final public function assertThat(
    mixed $value,
    ConstraintInterface $constraint,
    string $message = '',
  ): bool {

    return AssertThat::evaluate($this, $value, $constraint, $message);

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
  // public function assertTrue(bool $condition, string $message = ''): bool {
  //   $constraint = ConstraintFactory::factory('IsTrue');
  //   return $this->assertThat($condition, $constraint, $message);
  // }

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
  final public function readAttribute(
    mixed $classOrObject,
    string $attributeName,
  ): mixed {

    return ReadAttribute::evaluate($this, $classOrObject, $attributeName);

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
  final public function getStaticAttribute(
    string $className,
    string $attributeName,
  ): mixed {

    return GetStaticAttribute::evaluate($className, $attributeName);

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
  final public function getObjectAttribute(
    mixed $object,
    string $attributeName,
  ): mixed {

    return GetObjectAttribute::evaluate($object, $attributeName);

  }

}
