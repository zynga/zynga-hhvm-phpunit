<?php

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use Zynga\Framework\ReflectionCache\V1\ReflectionClasses;

use SebastianBergmann\PHPUnit\Assertions;
use SebastianBergmann\PHPUnit\AssertionsFactory;
use SebastianBergmann\PHPUnit\Exceptions\AssertionFailedException;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentException;

// --
// We are in the process of porting all of these assertion bits into hacklang
// strict code.
//
// When we complete all the porting, we will be attempting removal of the base
//  level class of PHPUnit_Framework_Assert
//
// --
/**
 * A set of assert methods.
 *
 * @since Class available since Release 2.0.0
 */
abstract class PHPUnit_Framework_Assert extends Assertions {

  /**
   * Evaluates a PHPUnit_Framework_Constraint matcher object.
   *
   * @param mixed                        $value
   * @param PHPUnit_Framework_Constraint $constraint
   * @param string                       $message
   *
   * @since Method available since Release 3.0.0
   */
  public static function legacyAssertThat(
    $value,
    PHPUnit_Framework_Constraint $constraint,
    $message = ''
  ) {

    $assertions = AssertionsFactory::factory();
    $assertions->counter()->add(count($constraint));

    $constraint->evaluate($value, $message);
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_Or matcher object.
   *
   * @return PHPUnit_Framework_Constraint_Or
   *
   * @since Method available since Release 3.0.0
   */
  public static function logicalOr() {
    $constraints = func_get_args();

    $constraint = new PHPUnit_Framework_Constraint_Or();
    $constraint->setConstraints($constraints);

    return $constraint;
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
  public static function logicalNot(PHPUnit_Framework_Constraint $constraint) {
    return new PHPUnit_Framework_Constraint_Not($constraint);
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_Xor matcher object.
   *
   * @return PHPUnit_Framework_Constraint_Xor
   *
   * @since Method available since Release 3.0.0
   */
  public static function logicalXor() {
    $constraints = func_get_args();

    $constraint = new PHPUnit_Framework_Constraint_Xor();
    $constraint->setConstraints($constraints);

    return $constraint;
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_IsAnything matcher object.
   *
   * @return PHPUnit_Framework_Constraint_IsAnything
   *
   * @since Method available since Release 3.0.0
   */
  public static function anything() {
    return new PHPUnit_Framework_Constraint_IsAnything();
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_IsTrue matcher object.
   *
   * @return PHPUnit_Framework_Constraint_IsTrue
   *
   * @since Method available since Release 3.3.0
   */
  public static function isTrue() {
    return new PHPUnit_Framework_Constraint_IsTrue();
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_Callback matcher object.
   *
   * @param callable $callback
   *
   * @return PHPUnit_Framework_Constraint_Callback
   */
  public static function callback($callback) {
    return new PHPUnit_Framework_Constraint_Callback($callback);
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_IsFalse matcher object.
   *
   * @return PHPUnit_Framework_Constraint_IsFalse
   *
   * @since Method available since Release 3.3.0
   */
  public static function isFalse() {
    return new PHPUnit_Framework_Constraint_IsFalse();
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_IsJson matcher object.
   *
   * @return PHPUnit_Framework_Constraint_IsJson
   *
   * @since Method available since Release 3.7.20
   */
  public static function isJson() {
    return new PHPUnit_Framework_Constraint_IsJson();
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_IsNull matcher object.
   *
   * @return PHPUnit_Framework_Constraint_IsNull
   *
   * @since Method available since Release 3.3.0
   */
  public static function isNull() {
    return new PHPUnit_Framework_Constraint_IsNull();
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_IsFinite matcher object.
   *
   * @return PHPUnit_Framework_Constraint_IsFinite
   *
   * @since Method available since Release 5.0.0
   */
  public static function isFinite() {
    return new PHPUnit_Framework_Constraint_IsFinite();
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_IsInfinite matcher object.
   *
   * @return PHPUnit_Framework_Constraint_IsInfinite
   *
   * @since Method available since Release 5.0.0
   */
  public static function isInfinite() {
    return new PHPUnit_Framework_Constraint_IsInfinite();
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_TraversableContains matcher
   * object.
   *
   * @param mixed $value
   * @param bool  $checkForObjectIdentity
   * @param bool  $checkForNonObjectIdentity
   *
   * @return PHPUnit_Framework_Constraint_TraversableContains
   *
   * @since Method available since Release 3.0.0
   */
  public static function contains(
    $value,
    $checkForObjectIdentity = true,
    $checkForNonObjectIdentity = false
  ) {
    return new PHPUnit_Framework_Constraint_TraversableContains(
      $value,
      $checkForObjectIdentity,
      $checkForNonObjectIdentity
    );
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_TraversableContainsOnly matcher
   * object.
   *
   * @param string $type
   *
   * @return PHPUnit_Framework_Constraint_TraversableContainsOnly
   *
   * @since Method available since Release 3.1.4
   */
  public static function containsOnly($type) {
    return new PHPUnit_Framework_Constraint_TraversableContainsOnly($type);
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_TraversableContainsOnly matcher
   * object.
   *
   * @param string $classname
   *
   * @return PHPUnit_Framework_Constraint_TraversableContainsOnly
   */
  public static function containsOnlyInstancesOf($classname) {
    return new PHPUnit_Framework_Constraint_TraversableContainsOnly(
      $classname,
      false
    );
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_ArrayHasKey matcher object.
   *
   * @param mixed $key
   *
   * @return PHPUnit_Framework_Constraint_ArrayHasKey
   *
   * @since Method available since Release 3.0.0
   */
  public static function arrayHasKey($key) {
    return new PHPUnit_Framework_Constraint_ArrayHasKey($key);
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_ClassHasAttribute matcher object.
   *
   * @param string $attributeName
   *
   * @return PHPUnit_Framework_Constraint_ClassHasAttribute
   *
   * @since Method available since Release 3.1.0
   */
  public static function classHasAttribute($attributeName) {
    return new PHPUnit_Framework_Constraint_ClassHasAttribute($attributeName);
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_ClassHasStaticAttribute matcher
   * object.
   *
   * @param string $attributeName
   *
   * @return PHPUnit_Framework_Constraint_ClassHasStaticAttribute
   *
   * @since Method available since Release 3.1.0
   */
  public static function classHasStaticAttribute($attributeName) {
    return new PHPUnit_Framework_Constraint_ClassHasStaticAttribute(
      $attributeName
    );
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_ObjectHasAttribute matcher object.
   *
   * @param string $attributeName
   *
   * @return PHPUnit_Framework_Constraint_ObjectHasAttribute
   *
   * @since Method available since Release 3.0.0
   */
  public static function objectHasAttribute($attributeName) {
    return
      new PHPUnit_Framework_Constraint_ObjectHasAttribute($attributeName);
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_IsIdentical matcher object.
   *
   * @param mixed $value
   *
   * @return PHPUnit_Framework_Constraint_IsIdentical
   *
   * @since Method available since Release 3.0.0
   */
  public static function identicalTo($value) {
    return new PHPUnit_Framework_Constraint_IsIdentical($value);
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_IsInstanceOf matcher object.
   *
   * @param string $className
   *
   * @return PHPUnit_Framework_Constraint_IsInstanceOf
   *
   * @since Method available since Release 3.0.0
   */
  public static function isInstanceOf($className) {
    return new PHPUnit_Framework_Constraint_IsInstanceOf($className);
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_IsType matcher object.
   *
   * @param string $type
   *
   * @return PHPUnit_Framework_Constraint_IsType
   *
   * @since Method available since Release 3.0.0
   */
  public static function isType($type) {
    return new PHPUnit_Framework_Constraint_IsType($type);
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_LessThan matcher object.
   *
   * @param mixed $value
   *
   * @return PHPUnit_Framework_Constraint_LessThan
   *
   * @since Method available since Release 3.0.0
   */
  public static function lessThan($value) {
    return new PHPUnit_Framework_Constraint_LessThan($value);
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_Or matcher object that wraps
   * a PHPUnit_Framework_Constraint_IsEqual and a
   * PHPUnit_Framework_Constraint_LessThan matcher object.
   *
   * @param mixed $value
   *
   * @return PHPUnit_Framework_Constraint_Or
   *
   * @since Method available since Release 3.1.0
   */
  public static function lessThanOrEqual($value) {
    return static::logicalOr(
      new PHPUnit_Framework_Constraint_IsEqual($value),
      new PHPUnit_Framework_Constraint_LessThan($value)
    );
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_PCREMatch matcher object.
   *
   * @param string $pattern
   *
   * @return PHPUnit_Framework_Constraint_PCREMatch
   *
   * @since Method available since Release 3.0.0
   */
  public static function matchesRegularExpression($pattern) {
    return new PHPUnit_Framework_Constraint_PCREMatch($pattern);
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_StringMatches matcher object.
   *
   * @param string $string
   *
   * @return PHPUnit_Framework_Constraint_StringMatches
   *
   * @since Method available since Release 3.5.0
   */
  public static function matches($string) {
    return new PHPUnit_Framework_Constraint_StringMatches($string);
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_StringStartsWith matcher object.
   *
   * @param mixed $prefix
   *
   * @return PHPUnit_Framework_Constraint_StringStartsWith
   *
   * @since Method available since Release 3.4.0
   */
  public static function stringStartsWith($prefix) {
    return new PHPUnit_Framework_Constraint_StringStartsWith($prefix);
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_StringContains matcher object.
   *
   * @param string $string
   * @param bool   $case
   *
   * @return PHPUnit_Framework_Constraint_StringContains
   *
   * @since Method available since Release 3.0.0
   */
  public static function stringContains($string, $case = true) {
    return new PHPUnit_Framework_Constraint_StringContains($string, $case);
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_StringEndsWith matcher object.
   *
   * @param mixed $suffix
   *
   * @return PHPUnit_Framework_Constraint_StringEndsWith
   *
   * @since Method available since Release 3.4.0
   */
  public static function stringEndsWith($suffix) {
    return new PHPUnit_Framework_Constraint_StringEndsWith($suffix);
  }

  /**
   * Returns a PHPUnit_Framework_Constraint_Count matcher object.
   *
   * @param int $count
   *
   * @return PHPUnit_Framework_Constraint_Count
   */
  public static function countOf($count) {
    return new PHPUnit_Framework_Constraint_Count($count);
  }

  /**
   * Fails a test with the given message.
   *
   * @param string $message
   *
   * @throws PHPUnit_Framework_AssertionFailedError
   */
  private static function legacyFail($message = '') {
     $assertions = AssertionsFactory::factory();
     return $assertions->fail($message);
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
  public static function legacyReadAttribute($classOrObject, $attributeName) {
    if (!is_string($attributeName)) {
      throw PHPUnit_Util_InvalidArgumentHelper::factory(2, 'string');
    }

    if (!preg_match(
          '/[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*/',
          $attributeName
        )) {
      throw PHPUnit_Util_InvalidArgumentHelper::factory(
        2,
        'valid attribute name'
      );
    }

    if (is_string($classOrObject)) {
      if (!class_exists($classOrObject)) {
        throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'class name');
      }

      return static::legacyGetStaticAttribute($classOrObject, $attributeName);
    } else if (is_object($classOrObject)) {
      return static::legacyGetObjectAttribute($classOrObject, $attributeName);
    } else {
      throw PHPUnit_Util_InvalidArgumentHelper::factory(
        1,
        'class name or object'
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
  private static function legacyGetStaticAttribute($className, $attributeName) {
    if (!is_string($className)) {
      throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'string');
    }

    if (!class_exists($className)) {
      throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'class name');
    }

    if (!is_string($attributeName)) {
      throw PHPUnit_Util_InvalidArgumentHelper::factory(2, 'string');
    }

    if (!preg_match(
          '/[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*/',
          $attributeName
        )) {
      throw PHPUnit_Util_InvalidArgumentHelper::factory(
        2,
        'valid attribute name'
      );
    }

    $class = ReflectionClasses::getReflection($className);

    while ($class) {
      $attributes = $class->getStaticProperties();

      if (array_key_exists($attributeName, $attributes)) {
        return $attributes[$attributeName];
      }

      $class = $class->getParentClass();
    }

    throw new PHPUnit_Framework_Exception(
      sprintf('Attribute "%s" not found in class.', $attributeName)
    );
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
  private static function legacyGetObjectAttribute($object, $attributeName) {
    if (!is_object($object)) {
      throw PHPUnit_Util_InvalidArgumentHelper::factory(1, 'object');
    }

    if (!is_string($attributeName)) {
      throw PHPUnit_Util_InvalidArgumentHelper::factory(2, 'string');
    }

    if (!preg_match(
          '/[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*/',
          $attributeName
        )) {
      throw PHPUnit_Util_InvalidArgumentHelper::factory(
        2,
        'valid attribute name'
      );
    }

    try {
      $attribute = new ReflectionProperty($object, $attributeName);
    } catch (ReflectionException $e) {
      $reflector = new ReflectionObject($object);

      while ($reflector = $reflector->getParentClass()) {
        try {
          $attribute = $reflector->getProperty($attributeName);
          break;
        } catch (ReflectionException $e) {
        }
      }
    }

    if (isset($attribute)) {
      if (!$attribute || $attribute->isPublic()) {
        return $object->$attributeName;
      }

      $attribute->setAccessible(true);
      $value = $attribute->getValue($object);
      $attribute->setAccessible(false);

      return $value;
    }

    throw new PHPUnit_Framework_Exception(
      sprintf('Attribute "%s" not found in object.', $attributeName)
    );
  }

  /**
   * Mark the test as incomplete.
   *
   * @param string $message
   *
   * @throws PHPUnit_Framework_IncompleteTestError
   *
   * @since Method available since Release 3.0.0
   */
  public static function markTestIncomplete($message = '') {
    throw new PHPUnit_Framework_IncompleteTestError($message);
  }

  /**
   * Mark the test as skipped.
   *
   * @param string $message
   *
   * @throws PHPUnit_Framework_SkippedTestError
   *
   * @since Method available since Release 3.0.0
   */
  public static function markTestSkipped($message = '') {
    throw new PHPUnit_Framework_SkippedTestError($message);
  }

}
