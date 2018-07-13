<?hh // strict

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use PHPUnit\Constraint\AndConstraint;
use PHPUnit\Constraint\ArrayHasKeyConstraint;
use PHPUnit\Constraint\ArraySubsetConstraint;
use PHPUnit\Constraint\AttributeConstraint;
use PHPUnit\Constraint\CallbackConstraint;
use PHPUnit\Constraint\ClassHasAttributeConstraint;
use PHPUnit\Constraint\ClassHasStaticAttributeConstraint;
use PHPUnit\Constraint\CountConstraint;
// Ported, but unused at this time.
//use PHPUnit\Constraint\ExceptionCodeConstraint;
//use PHPUnit\Constraint\ExceptionConstraint;
//use PHPUnit\Constraint\ExceptionMessageConstraint;
//use PHPUnit\Constraint\ExceptionMessageRegExpConstraint;
use PHPUnit\Constraint\FileExistsConstraint;
use PHPUnit\Constraint\GreaterThanConstraint;
use PHPUnit\Constraint\IsAnythingConstraint;
use PHPUnit\Constraint\IsEmptyConstraint;
use PHPUnit\Constraint\IsEqualConstraint;
use PHPUnit\Constraint\IsFalseConstraint;
use PHPUnit\Constraint\IsFiniteConstraint;
use PHPUnit\Constraint\IsIdenticalConstraint;
use PHPUnit\Constraint\IsInfiniteConstraint;
use PHPUnit\Constraint\IsInstanceOfConstraint;
use PHPUnit\Constraint\IsJsonConstraint;
use PHPUnit\Constraint\IsNanConstraint;
use PHPUnit\Constraint\IsNullConstraint;
use PHPUnit\Constraint\IsTrueConstraint;
use PHPUnit\Constraint\IsTypeConstraint;
use PHPUnit\Constraint\JsonMatchesConstraint;
use PHPUnit\Constraint\LessThanConstraint;
use PHPUnit\Constraint\NotConstraint;
use PHPUnit\Constraint\ObjectHasAttributeConstraint;
use PHPUnit\Constraint\OrConstraint;
use PHPUnit\Constraint\PCREMatchConstraint;
use PHPUnit\Constraint\SameSizeConstraint;
use PHPUnit\Constraint\StringContainsConstraint;
use PHPUnit\Constraint\StringEndsWithConstraint;
use PHPUnit\Constraint\StringMatchesConstraint;
use PHPUnit\Constraint\StringStartsWithConstraint;
use PHPUnit\Constraint\TraversableContainsConstraint;
use PHPUnit\Constraint\TraversableContainsOnlyConstraint;
use PHPUnit\Constraint\XorConstraint;

use PHPUnit\Exceptions\AssertionFailedError;
use PHPUnit\Exceptions\Exception as PHPUnit_Exceptions_Exception;
use PHPUnit\Exceptions\Error\IncompleteTestError;
use PHPUnit\Exceptions\SkippedTestError;
use PHPUnit\Interfaces\ConstraintInterface;
use PHPUnit\Util\TypeUtil;
use PHPUnit\Util\InvalidArgumentHelperUtil;
use PHPUnit\Util\XMLUtil;

/**
 * A set of assert methods.
 *
 * @since Class available since Release 2.0.0
 */
abstract class PHPUnit_Framework_Assert {

    /**
     * @var int
     */
    private int $count = 0;

    /**
     * Asserts that an array has a specified key.
     *
     * @param mixed             $key
     * @param array|ArrayAccess $array
     * @param string            $message
     *
     * @since Method available since Release 3.0.0
     */
    public function assertArrayHasKey(mixed $key, mixed $array, string $message = ''): void {

        if (!(is_array($array) || $array instanceof ArrayAccess)) {
            throw InvalidArgumentHelperUtil::factory(
                2,
                'array or ArrayAccess'
            );
        }

        if ( is_int($key) ) {
          $constraint = new ArrayHasKeyConstraint($key);
          $this->assertThat($array, $constraint, $message);
        } else {
          throw InvalidArgumentHelperUtil::factory(
              1,
              'expected integer'
          );
        }

        if ( is_string($key) ) {
          $constraint = new ArrayHasKeyConstraint($key);
          $this->assertThat($array, $constraint, $message);
        } else {
          throw InvalidArgumentHelperUtil::factory(
              1,
              'expected string'
          );
        }

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
    public function assertArraySubset(mixed $subset, mixed $array, bool $strict = false, string $message = ''): void {

      if (!is_array($subset)) {
        throw InvalidArgumentHelperUtil::factory(
          1,
          'array or ArrayAccess'
        );
      }

      if (!is_array($array)) {
        throw InvalidArgumentHelperUtil::factory(
          2,
          'array or ArrayAccess'
        );
      }

      $constraint = new ArraySubsetConstraint($subset, $strict);

      $this->assertThat($array, $constraint, $message);

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
    public function assertArrayNotHasKey(mixed $key, mixed $array, string $message = ''): void {
        if (!(is_int($key) || is_string($key))) {
            throw InvalidArgumentHelperUtil::factory(
                1,
                'integer or string'
            );
        }

        if (!(is_array($array) || $array instanceof ArrayAccess)) {
            throw InvalidArgumentHelperUtil::factory(
                2,
                'array or ArrayAccess'
            );
        }

        $constraint = new NotConstraint(
            new ArrayHasKeyConstraint($key)
        );

        $this->assertThat($array, $constraint, $message);
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
    public function assertContains(mixed $needle, mixed $haystack, string $message = '', bool $ignoreCase = false, bool $checkForObjectIdentity = true, bool $checkForNonObjectIdentity = false): void {
        if (is_array($haystack) ||
            is_object($haystack) && $haystack instanceof Traversable) {
            $constraint = new TraversableContainsConstraint(
                $needle,
                $checkForObjectIdentity,
                $checkForNonObjectIdentity
            );
        } elseif (is_string($haystack)) {
            if (!is_string($needle)) {
                throw InvalidArgumentHelperUtil::factory(
                    1,
                    'string'
                );
            }

            $constraint = new StringContainsConstraint(
                $needle,
                $ignoreCase
            );
        } else {
            throw InvalidArgumentHelperUtil::factory(
                2,
                'array, traversable or string'
            );
        }

        $this->assertThat($haystack, $constraint, $message);
    }

    /**
     * Asserts that a haystack that is stored in a attribute of a class
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
    public function assertAttributeContains(mixed $needle, string $haystackAttributeName, mixed $haystackClassOrObject, string $message = '', bool $ignoreCase = false, bool $checkForObjectIdentity = true, bool $checkForNonObjectIdentity = false): void {
      $this->assertContains(
        $needle,
        $this->readAttribute($haystackClassOrObject, $haystackAttributeName),
        $message,
        $ignoreCase,
        $checkForObjectIdentity,
        $checkForNonObjectIdentity
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
    public function assertNotContains(mixed $needle, mixed $haystack, string $message = '', bool $ignoreCase = false, bool $checkForObjectIdentity = true, bool $checkForNonObjectIdentity = false): void {

      if (is_array($haystack) ||
          is_object($haystack) && $haystack instanceof Traversable) {

        $constraint = new NotConstraint(
          new TraversableContainsConstraint(
            $needle,
            $checkForObjectIdentity,
            $checkForNonObjectIdentity
          )
        );

      } elseif (is_string($haystack)) {

        if (!is_string($needle)) {
          throw InvalidArgumentHelperUtil::factory(
            1,
            'string'
          );
        }

        $constraint = new NotConstraint(
          new StringContainsConstraint(
            $needle,
            $ignoreCase
          )
        );
      } else {
          throw InvalidArgumentHelperUtil::factory(
            2,
            'array, traversable or string'
          );
      }

      $this->assertThat($haystack, $constraint, $message);

    }

    /**
     * Asserts that a haystack that is stored in a attribute of a class
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
    public function assertAttributeNotContains(mixed $needle, string $haystackAttributeName, mixed $haystackClassOrObject, string $message = '', bool $ignoreCase = false, bool $checkForObjectIdentity = true, bool $checkForNonObjectIdentity = false): void {
      $this->assertNotContains(
        $needle,
        $this->readAttribute($haystackClassOrObject, $haystackAttributeName),
        $message,
        $ignoreCase,
        $checkForObjectIdentity,
        $checkForNonObjectIdentity
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
    public function assertContainsOnly(string $type, mixed $haystack, ?bool $isNativeType = null, string $message = ''): void {

      if (!(is_array($haystack) ||
          is_object($haystack) && $haystack instanceof Traversable)) {
          throw InvalidArgumentHelperUtil::factory(
              2,
              'array or traversable'
          );
      }

      if ($isNativeType === null) {
        $isNativeType = TypeUtil::isType($type);
      }

      $this->assertThat(
        $haystack,
        new TraversableContainsOnlyConstraint(
          $type,
          $isNativeType
        ),
        $message
      );

    }

    /**
     * Asserts that a haystack contains only instances of a given classname
     *
     * @param string            $classname
     * @param array|Traversable $haystack
     * @param string            $message
     */
    public function assertContainsOnlyInstancesOf(string $classname, mixed $haystack, string $message = ''): void {

      if (!(is_array($haystack) ||
          is_object($haystack) && $haystack instanceof Traversable)) {
          throw InvalidArgumentHelperUtil::factory(
            2,
            'array or traversable'
          );
      }

      $this->assertThat(
        $haystack,
        new TraversableContainsOnlyConstraint(
          $classname,
          false
        ),
        $message
      );

    }

    /**
     * Asserts that a haystack that is stored in a attribute of a class
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
    public function assertAttributeContainsOnly(string $type, string $haystackAttributeName, mixed $haystackClassOrObject, ?bool $isNativeType = null, string $message = ''): void {

      $this->assertContainsOnly(
        $type,
        $this->readAttribute($haystackClassOrObject, $haystackAttributeName),
        $isNativeType,
        $message
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
    public function assertNotContainsOnly(string $type, mixed $haystack, ?bool $isNativeType = null, string $message = ''): void {

      if (!(is_array($haystack) ||
        is_object($haystack) && $haystack instanceof Traversable)) {
        throw InvalidArgumentHelperUtil::factory(
          2,
          'array or traversable'
        );
      }

      if ($isNativeType === null) {
        $isNativeType = TypeUtil::isType($type);
      }

      $this->assertThat(
        $haystack,
        new NotConstraint(
          new TraversableContainsOnlyConstraint(
            $type,
            $isNativeType
          )
        ),
        $message
      );

    }

    /**
     * Asserts that a haystack that is stored in a attribute of a class
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
    public function assertAttributeNotContainsOnly(string $type, string $haystackAttributeName, mixed $haystackClassOrObject, ?bool $isNativeType = null, string $message = ''): void {

      $this->assertNotContainsOnly(
        $type,
        $this->readAttribute($haystackClassOrObject, $haystackAttributeName),
        $isNativeType,
        $message
      );

    }

    /**
     * Asserts the number of elements of an array, Countable or Traversable.
     *
     * @param int    $expectedCount
     * @param mixed  $haystack
     * @param string $message
     */
    public function assertCount(int $expectedCount, mixed $haystack, string $message = ''): void {

      if (!$haystack instanceof Countable &&
        !$haystack instanceof Traversable &&
        !is_array($haystack)) {
        throw InvalidArgumentHelperUtil::factory(2, 'countable or traversable');
      }

      $this->assertThat(
        $haystack,
        new CountConstraint($expectedCount),
        $message
      );

    }

    /**
     * Asserts the number of elements of an array, Countable or Traversable
     * that is stored in an attribute.
     *
     * @param int           $expectedCount
     * @param string        $haystackAttributeName
     * @param string|object $haystackClassOrObject
     * @param string        $message
     *
     * @since Method available since Release 3.6.0
     */
    public function assertAttributeCount(int $expectedCount, string $haystackAttributeName, mixed $haystackClassOrObject, string $message = ''): void {

      $this->assertCount(
        $expectedCount,
        $this->readAttribute($haystackClassOrObject, $haystackAttributeName),
        $message
      );

    }

    /**
     * Asserts the number of elements of an array, Countable or Traversable.
     *
     * @param int    $expectedCount
     * @param mixed  $haystack
     * @param string $message
     */
    public function assertNotCount(int $expectedCount, mixed $haystack, string $message = ''): void {

      if (!$haystack instanceof Countable &&
        !$haystack instanceof Traversable &&
        !is_array($haystack)) {
        throw InvalidArgumentHelperUtil::factory(2, 'countable or traversable');
      }

      $constraint = new NotConstraint(
        new CountConstraint($expectedCount)
      );

      $this->assertThat($haystack, $constraint, $message);

    }

    /**
     * Asserts the number of elements of an array, Countable or Traversable
     * that is stored in an attribute.
     *
     * @param int           $expectedCount
     * @param string        $haystackAttributeName
     * @param string|object $haystackClassOrObject
     * @param string        $message
     *
     * @since Method available since Release 3.6.0
     */
    public function assertAttributeNotCount(int $expectedCount, string $haystackAttributeName, mixed $haystackClassOrObject, string $message = ''): void {

      $this->assertNotCount(
        $expectedCount,
        $this->readAttribute($haystackClassOrObject, $haystackAttributeName),
        $message
      );

    }

    /**
     * Asserts that two variables are equal.
     *
     * @param mixed  $expected
     * @param mixed  $actual
     * @param string $message
     * @param float  $delta
     * @param int    $maxDepth
     * @param bool   $canonicalize
     * @param bool   $ignoreCase
     */
    public function assertEquals(mixed $expected, mixed $actual, string $message = '', float $delta = 0.0, int $maxDepth = 10, bool $canonicalize = false, bool $ignoreCase = false): void {

      $constraint = new IsEqualConstraint(
        $expected,
        $delta,
        $maxDepth,
        $canonicalize,
        $ignoreCase
      );

      $this->assertThat($actual, $constraint, $message);

    }

    /**
     * Asserts that a variable is equal to an attribute of an object.
     *
     * @param mixed         $expected
     * @param string        $actualAttributeName
     * @param string|object $actualClassOrObject
     * @param string        $message
     * @param float         $delta
     * @param int           $maxDepth
     * @param bool          $canonicalize
     * @param bool          $ignoreCase
     */
    public function assertAttributeEquals(mixed $expected, string $actualAttributeName, mixed $actualClassOrObject, string $message = '', float $delta = 0.0, int $maxDepth = 10, bool $canonicalize = false, bool $ignoreCase = false): void {

      $this->assertEquals(
        $expected,
        $this->readAttribute($actualClassOrObject, $actualAttributeName),
        $message,
        $delta,
        $maxDepth,
        $canonicalize,
        $ignoreCase
      );

    }

    /**
     * Asserts that two variables are not equal.
     *
     * @param mixed  $expected
     * @param mixed  $actual
     * @param string $message
     * @param float  $delta
     * @param int    $maxDepth
     * @param bool   $canonicalize
     * @param bool   $ignoreCase
     *
     * @since Method available since Release 2.3.0
     */
    public function assertNotEquals(mixed $expected, mixed $actual, string $message = '', float $delta = 0.0, int $maxDepth = 10, bool $canonicalize = false, bool $ignoreCase = false): void {

      $constraint = new NotConstraint(
        new IsEqualConstraint(
          $expected,
          $delta,
          $maxDepth,
          $canonicalize,
          $ignoreCase
        )
      );

      $this->assertThat($actual, $constraint, $message);

    }

    /**
     * Asserts that a variable is not equal to an attribute of an object.
     *
     * @param mixed         $expected
     * @param string        $actualAttributeName
     * @param string|object $actualClassOrObject
     * @param string        $message
     * @param float         $delta
     * @param int           $maxDepth
     * @param bool          $canonicalize
     * @param bool          $ignoreCase
     */
    public function assertAttributeNotEquals(mixed $expected, string $actualAttributeName, mixed $actualClassOrObject, string $message = '', float $delta = 0.0, int $maxDepth = 10, bool $canonicalize = false, bool $ignoreCase = false): void {

      $this->assertNotEquals(
        $expected,
        $this->readAttribute($actualClassOrObject, $actualAttributeName),
        $message,
        $delta,
        $maxDepth,
        $canonicalize,
        $ignoreCase
      );

    }

    /**
     * Asserts that a variable is empty.
     *
     * @param mixed  $actual
     * @param string $message
     *
     * @throws AssertionFailedError
     */
    public function assertEmpty(mixed $actual, string $message = ''): void {
      $this->assertThat($actual, $this->isEmpty(), $message);
    }

    /**
     * Asserts that a attribute of a class or an attribute of an object
     * is empty.
     *
     * @param string        $haystackAttributeName
     * @param string|object $haystackClassOrObject
     * @param string        $message
     *
     * @since Method available since Release 3.5.0
     */
    public function assertAttributeEmpty(string $haystackAttributeName, mixed $haystackClassOrObject, string $message = ''): void {

      $this->assertEmpty(
        $this->readAttribute($haystackClassOrObject, $haystackAttributeName),
        $message
      );

    }

    /**
     * Asserts that a variable is not empty.
     *
     * @param mixed  $actual
     * @param string $message
     *
     * @throws AssertionFailedError
     */
    public function assertNotEmpty(mixed $actual, string $message = ''): void {
      $this->assertThat($actual, $this->logicalNot($this->isEmpty()), $message);
    }

    /**
     * Asserts that a attribute of a class or an attribute of an object
     * is not empty.
     *
     * @param string        $haystackAttributeName
     * @param string|object $haystackClassOrObject
     * @param string        $message
     *
     * @since Method available since Release 3.5.0
     */
    public function assertAttributeNotEmpty(string $haystackAttributeName, mixed $haystackClassOrObject, string $message = ''): void {

      $this->assertNotEmpty(
        $this->readAttribute($haystackClassOrObject, $haystackAttributeName),
        $message
      );

    }

    /**
     * Asserts that a value is greater than another value.
     *
     * @param mixed  $expected
     * @param mixed  $actual
     * @param string $message
     *
     * @since Method available since Release 3.1.0
     */
    public function assertGreaterThan(mixed $expected, mixed $actual, string $message = ''): void {
      $this->assertThat($actual, $this->greaterThan($expected), $message);
    }

    /**
     * Asserts that an attribute is greater than another value.
     *
     * @param mixed         $expected
     * @param string        $actualAttributeName
     * @param string|object $actualClassOrObject
     * @param string        $message
     *
     * @since Method available since Release 3.1.0
     */
    public  function assertAttributeGreaterThan(mixed $expected, string $actualAttributeName, mixed $actualClassOrObject, string $message = ''): void {

      $this->assertGreaterThan(
        $expected,
        $this->readAttribute($actualClassOrObject, $actualAttributeName),
        $message
      );

    }

    /**
     * Asserts that a value is greater than or equal to another value.
     *
     * @param mixed  $expected
     * @param mixed  $actual
     * @param string $message
     *
     * @since Method available since Release 3.1.0
     */
    public function assertGreaterThanOrEqual(mixed $expected, mixed $actual, string $message = ''): void {

      $this->assertThat(
        $actual,
        $this->greaterThanOrEqual($expected),
        $message
      );

    }

    /**
     * Asserts that an attribute is greater than or equal to another value.
     *
     * @param mixed         $expected
     * @param string        $actualAttributeName
     * @param string|object $actualClassOrObject
     * @param string        $message
     *
     * @since Method available since Release 3.1.0
     */
    public function assertAttributeGreaterThanOrEqual(mixed $expected, string $actualAttributeName, mixed $actualClassOrObject, string $message = ''): void {

      $this->assertGreaterThanOrEqual(
        $expected,
        $this->readAttribute($actualClassOrObject, $actualAttributeName),
        $message
      );

    }

    /**
     * Asserts that a value is smaller than another value.
     *
     * @param mixed  $expected
     * @param mixed  $actual
     * @param string $message
     *
     * @since Method available since Release 3.1.0
     */
    public function assertLessThan(mixed $expected, mixed $actual, string $message = ''): void {

      $this->assertThat($actual, $this->lessThan($expected), $message);

    }

    /**
     * Asserts that an attribute is smaller than another value.
     *
     * @param mixed         $expected
     * @param string        $actualAttributeName
     * @param string|object $actualClassOrObject
     * @param string        $message
     *
     * @since Method available since Release 3.1.0
     */
    public function assertAttributeLessThan(mixed $expected, string $actualAttributeName, mixed $actualClassOrObject, string $message = ''): void {

      $this->assertLessThan(
        $expected,
        $this->readAttribute($actualClassOrObject, $actualAttributeName),
        $message
      );

    }

    /**
     * Asserts that a value is smaller than or equal to another value.
     *
     * @param mixed  $expected
     * @param mixed  $actual
     * @param string $message
     *
     * @since Method available since Release 3.1.0
     */
    public function assertLessThanOrEqual(mixed $expected, mixed $actual, string $message = ''): void {

      $this->assertThat($actual, $this->lessThanOrEqual($expected), $message);

    }

    /**
     * Asserts that an attribute is smaller than or equal to another value.
     *
     * @param mixed         $expected
     * @param string        $actualAttributeName
     * @param string|object $actualClassOrObject
     * @param string        $message
     *
     * @since Method available since Release 3.1.0
     */
    public function assertAttributeLessThanOrEqual(mixed $expected, string $actualAttributeName, mixed $actualClassOrObject, string $message = ''): void {

      $this->assertLessThanOrEqual(
        $expected,
        $this->readAttribute($actualClassOrObject, $actualAttributeName),
        $message
      );

    }

    /**
     * Asserts that the contents of one file is equal to the contents of another
     * file.
     *
     * @param string $expected
     * @param string $actual
     * @param string $message
     * @param bool   $canonicalize
     * @param bool   $ignoreCase
     *
     * @since Method available since Release 3.2.14
     */
    public function assertFileEquals(string $expected, string $actual, string $message = '', bool $canonicalize = false, bool $ignoreCase = false): void {

      $this->assertFileExists($expected, $message);
      $this->assertFileExists($actual, $message);

      $this->assertEquals(
        file_get_contents($expected),
        file_get_contents($actual),
        $message,
        0.0,
        10,
        $canonicalize,
        $ignoreCase
      );

    }

    /**
     * Asserts that the contents of one file is not equal to the contents of
     * another file.
     *
     * @param string $expected
     * @param string $actual
     * @param string $message
     * @param bool   $canonicalize
     * @param bool   $ignoreCase
     *
     * @since Method available since Release 3.2.14
     */
    public function assertFileNotEquals(string $expected, string $actual, string $message = '', bool $canonicalize = false, bool $ignoreCase = false): void {

      $this->assertFileExists($expected, $message);
      $this->assertFileExists($actual, $message);

      $this->assertNotEquals(
        file_get_contents($expected),
        file_get_contents($actual),
        $message,
        0.0,
        10,
        $canonicalize,
        $ignoreCase
      );

    }

    /**
     * Asserts that the contents of a string is equal
     * to the contents of a file.
     *
     * @param string $expectedFile
     * @param string $actualString
     * @param string $message
     * @param bool   $canonicalize
     * @param bool   $ignoreCase
     *
     * @since Method available since Release 3.3.0
     */
    public function assertStringEqualsFile(string $expectedFile, string $actualString, string $message = '', bool $canonicalize = false, bool $ignoreCase = false): void {

      $this->assertFileExists($expectedFile, $message);

      $this->assertEquals(
        file_get_contents($expectedFile),
        $actualString,
        $message,
        0.0,
        10,
        $canonicalize,
        $ignoreCase
      );

    }

    /**
     * Asserts that the contents of a string is not equal
     * to the contents of a file.
     *
     * @param string $expectedFile
     * @param string $actualString
     * @param string $message
     * @param bool   $canonicalize
     * @param bool   $ignoreCase
     *
     * @since Method available since Release 3.3.0
     */
    public function assertStringNotEqualsFile(string $expectedFile, string $actualString, string $message = '', bool $canonicalize = false, bool $ignoreCase = false): void {

      $this->assertFileExists($expectedFile, $message);

      $this->assertNotEquals(
        file_get_contents($expectedFile),
        $actualString,
        $message,
        0.0,
        10,
        $canonicalize,
        $ignoreCase
      );

    }

    /**
     * Asserts that a file exists.
     *
     * @param string $filename
     * @param string $message
     *
     * @since Method available since Release 3.0.0
     */
    public function assertFileExists(string $filename, string $message = ''): void {

      $constraint = new FileExistsConstraint();

      $this->assertThat($filename, $constraint, $message);

    }

    /**
     * Asserts that a file does not exist.
     *
     * @param string $filename
     * @param string $message
     *
     * @since Method available since Release 3.0.0
     */
    public function assertFileNotExists(string $filename, string $message = ''): void {

      $constraint = new NotConstraint(
        new FileExistsConstraint()
      );

      $this->assertThat($filename, $constraint, $message);

    }

    /**
     * Asserts that a condition is true.
     *
     * @param bool   $condition
     * @param string $message
     *
     * @throws AssertionFailedError
     */
    public function assertTrue(bool $condition, string $message = ''): void {
      $this->assertThat($condition, $this->isTrue(), $message);
    }

    /**
     * Asserts that a condition is not true.
     *
     * @param bool   $condition
     * @param string $message
     *
     * @throws AssertionFailedError
     */
    public function assertNotTrue(bool $condition, string $message = ''): void {
      $this->assertThat($condition, $this->logicalNot($this->isTrue()), $message);
    }

    /**
     * Asserts that a condition is false.
     *
     * @param bool   $condition
     * @param string $message
     *
     * @throws AssertionFailedError
     */
    public function assertFalse(bool $condition, string $message = ''): void {

      $this->assertThat($condition, $this->isFalse(), $message);

    }

    /**
     * Asserts that a condition is not false.
     *
     * @param bool   $condition
     * @param string $message
     *
     * @throws AssertionFailedError
     */
    public function assertNotFalse(bool $condition, string $message = ''): void {

      $this->assertThat($condition, $this->logicalNot($this->isFalse()), $message);

    }

    /**
     * Asserts that a variable is null.
     *
     * @param mixed  $actual
     * @param string $message
     */
    public function assertNull(mixed $actual, string $message = ''): void {

      $this->assertThat($actual, $this->isNull(), $message);

    }

    /**
     * Asserts that a variable is not null.
     *
     * @param mixed  $actual
     * @param string $message
     */
    public function assertNotNull(mixed $actual, string $message = ''): void {

      $this->assertThat($actual, $this->logicalNot($this->isNull()), $message);

    }

    /**
     * Asserts that a variable is finite.
     *
     * @param mixed  $actual
     * @param string $message
     */
    public function assertFinite(mixed $actual, string $message = ''): void {

      $this->assertThat($actual, $this->isFinite(), $message);

    }

    /**
     * Asserts that a variable is infinite.
     *
     * @param mixed  $actual
     * @param string $message
     */
    public function assertInfinite(mixed $actual, string $message = ''): void {

      $this->assertThat($actual, $this->isInfinite(), $message);

    }

    /**
     * Asserts that a variable is nan.
     *
     * @param mixed  $actual
     * @param string $message
     */
    public function assertNan(mixed $actual, string $message = ''): void {

      $this->assertThat($actual, $this->isNan(), $message);

    }

    /**
     * Asserts that a class has a specified attribute.
     *
     * @param string $attributeName
     * @param string $className
     * @param string $message
     *
     * @since Method available since Release 3.1.0
     */
    public function assertClassHasAttribute(string $attributeName, string $className, string $message = ''): void {

      if (!preg_match('/[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*/', $attributeName)) {
        throw InvalidArgumentHelperUtil::factory(1, 'valid attribute name');
      }

      if (!is_string($className) || !class_exists($className)) {
        throw InvalidArgumentHelperUtil::factory(2, 'class name', $className);
      }

      $constraint = new ClassHasAttributeConstraint(
        $attributeName
      );

      $this->assertThat($className, $constraint, $message);

    }

    /**
     * Asserts that a class does not have a specified attribute.
     *
     * @param string $attributeName
     * @param string $className
     * @param string $message
     *
     * @since Method available since Release 3.1.0
     */
    public function assertClassNotHasAttribute(string $attributeName, string $className, string $message = ''): void {

      if (!preg_match('/[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*/', $attributeName)) {
        throw InvalidArgumentHelperUtil::factory(1, 'valid attribute name');
      }

      if (!is_string($className) || !class_exists($className)) {
        throw InvalidArgumentHelperUtil::factory(2, 'class name', $className);
      }

      $constraint = new NotConstraint(
        new ClassHasAttributeConstraint($attributeName)
      );

      $this->assertThat($className, $constraint, $message);

    }

    /**
     * Asserts that a class has a specified attribute.
     *
     * @param string $attributeName
     * @param string $className
     * @param string $message
     *
     * @since Method available since Release 3.1.0
     */
    public function assertClassHasStaticAttribute(string $attributeName, string $className, string $message = ''): void {

      if (!preg_match('/[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*/', $attributeName)) {
        throw InvalidArgumentHelperUtil::factory(1, 'valid attribute name');
      }

      if (!is_string($className) || !class_exists($className)) {
        throw InvalidArgumentHelperUtil::factory(2, 'class name', $className);
      }

      $constraint = new ClassHasStaticAttributeConstraint(
        $attributeName
      );

      $this->assertThat($className, $constraint, $message);

    }

    /**
     * Asserts that a class does not have a specified attribute.
     *
     * @param string $attributeName
     * @param string $className
     * @param string $message
     *
     * @since Method available since Release 3.1.0
     */
    public function assertClassNotHasStaticAttribute(string $attributeName, string $className, string $message = ''): void {

      if (!preg_match('/[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*/', $attributeName)) {
        throw InvalidArgumentHelperUtil::factory(1, 'valid attribute name');
      }

      if (!is_string($className) || !class_exists($className)) {
        throw InvalidArgumentHelperUtil::factory(2, 'class name', $className);
      }

      $constraint = new NotConstraint(
        new ClassHasStaticAttributeConstraint(
          $attributeName
        )
      );

      $this->assertThat($className, $constraint, $message);

    }

    /**
     * Asserts that an object has a specified attribute.
     *
     * @param string $attributeName
     * @param object $object
     * @param string $message
     *
     * @since Method available since Release 3.0.0
     */
    public function assertObjectHasAttribute(string $attributeName, mixed $object, string $message = ''): void {

      if (!preg_match('/[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*/', $attributeName)) {
        throw InvalidArgumentHelperUtil::factory(1, 'valid attribute name');
      }

      if (!is_object($object)) {
        throw InvalidArgumentHelperUtil::factory(2, 'object');
      }

      $constraint = new ObjectHasAttributeConstraint(
        $attributeName
      );

      $this->assertThat($object, $constraint, $message);

    }

    /**
     * Asserts that an object does not have a specified attribute.
     *
     * @param string $attributeName
     * @param object $object
     * @param string $message
     *
     * @since Method available since Release 3.0.0
     */
    public function assertObjectNotHasAttribute(string $attributeName, mixed $object, string $message = ''): void {

        if (!is_string($attributeName)) {
            throw InvalidArgumentHelperUtil::factory(1, 'string');
        }

        if (!preg_match('/[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*/', $attributeName)) {
            throw InvalidArgumentHelperUtil::factory(1, 'valid attribute name');
        }

        if (!is_object($object)) {
            throw InvalidArgumentHelperUtil::factory(2, 'object');
        }

        $constraint = new NotConstraint(
            new ObjectHasAttributeConstraint($attributeName)
        );

        $this->assertThat($object, $constraint, $message);

    }

    /**
     * Asserts that two variables have the same type and value.
     * Used on objects, it asserts that two variables reference
     * the same object.
     *
     * @param mixed  $expected
     * @param mixed  $actual
     * @param string $message
     */
    public function assertSame(mixed $expected, mixed $actual, string $message = ''): void {

      if (is_bool($expected) && is_bool($actual)) {

        $this->assertEquals($expected, $actual, $message);
      } else {

        $constraint = new IsIdenticalConstraint(
          $expected
        );

        $this->assertThat($actual, $constraint, $message);
      }

    }

    /**
     * Asserts that a variable and an attribute of an object have the same type
     * and value.
     *
     * @param mixed         $expected
     * @param string        $actualAttributeName
     * @param string|object $actualClassOrObject
     * @param string        $message
     */
    public function assertAttributeSame(mixed $expected, string $actualAttributeName, mixed $actualClassOrObject, string $message = ''): void {

      $this->assertSame(
        $expected,
        $this->readAttribute($actualClassOrObject, $actualAttributeName),
        $message
      );

    }

    /**
     * Asserts that two variables do not have the same type and value.
     * Used on objects, it asserts that two variables do not reference
     * the same object.
     *
     * @param mixed  $expected
     * @param mixed  $actual
     * @param string $message
     */
    public function assertNotSame(mixed $expected, mixed $actual, string $message = ''): void {

      if (is_bool($expected) && is_bool($actual)) {

        $this->assertNotEquals($expected, $actual, $message);

      } else {

        $constraint = new NotConstraint(
          new IsIdenticalConstraint($expected)
        );

        $this->assertThat($actual, $constraint, $message);
      }

    }

    /**
     * Asserts that a variable and an attribute of an object do not have the
     * same type and value.
     *
     * @param mixed         $expected
     * @param string        $actualAttributeName
     * @param string|object $actualClassOrObject
     * @param string        $message
     */
    public function assertAttributeNotSame(mixed $expected, string $actualAttributeName, mixed $actualClassOrObject, string $message = ''): void {

      $this->assertNotSame(
        $expected,
        $this->readAttribute($actualClassOrObject, $actualAttributeName),
        $message
      );

    }

    /**
     * Asserts that a variable is of a given type.
     *
     * @param string $expected
     * @param mixed  $actual
     * @param string $message
     *
     * @since Method available since Release 3.5.0
     */
    public function assertInstanceOf(string $expected, mixed $actual, string $message = ''): void {

      if (!(is_string($expected) && (class_exists($expected) || interface_exists($expected)))) {
        throw InvalidArgumentHelperUtil::factory(1, 'class or interface name');
      }

      $constraint = new IsInstanceOfConstraint(
        $expected
      );

      $this->assertThat($actual, $constraint, $message);

    }

    /**
     * Asserts that an attribute is of a given type.
     *
     * @param string        $expected
     * @param string        $attributeName
     * @param string|object $classOrObject
     * @param string        $message
     *
     * @since Method available since Release 3.5.0
     */
    public function assertAttributeInstanceOf(string $expected, string $attributeName, mixed $classOrObject, string $message = ''): void {

      $this->assertInstanceOf(
        $expected,
        $this->readAttribute($classOrObject, $attributeName),
        $message
      );

    }

    /**
     * Asserts that a variable is not of a given type.
     *
     * @param string $expected
     * @param mixed  $actual
     * @param string $message
     *
     * @since Method available since Release 3.5.0
     */
    public function assertNotInstanceOf(string $expected, mixed $actual, string $message = ''): void {

      $constraint = new NotConstraint(
        new IsInstanceOfConstraint($expected)
      );

      $this->assertThat($actual, $constraint, $message);

    }

    /**
     * Asserts that an attribute is of a given type.
     *
     * @param string        $expected
     * @param string        $attributeName
     * @param string|object $classOrObject
     * @param string        $message
     *
     * @since Method available since Release 3.5.0
     */
    public function assertAttributeNotInstanceOf(string $expected, string $attributeName, mixed $classOrObject, string $message = ''): void {

      $this->assertNotInstanceOf(
        $expected,
        $this->readAttribute($classOrObject, $attributeName),
        $message
      );

    }

    /**
     * Asserts that a variable is of a given type.
     *
     * @param string $expected
     * @param mixed  $actual
     * @param string $message
     *
     * @since Method available since Release 3.5.0
     */
    public function assertInternalType(string $expected, mixed $actual, string $message = ''): void {

      $constraint = new IsTypeConstraint(
        $expected
      );

      $this->assertThat($actual, $constraint, $message);

    }

    /**
     * Asserts that an attribute is of a given type.
     *
     * @param string        $expected
     * @param string        $attributeName
     * @param string|object $classOrObject
     * @param string        $message
     *
     * @since Method available since Release 3.5.0
     */
    public function assertAttributeInternalType(string $expected, string $attributeName, mixed $classOrObject, string $message = ''): void {

      $this->assertInternalType(
        $expected,
        $this->readAttribute($classOrObject, $attributeName),
        $message
      );

    }

    /**
     * Asserts that a variable is not of a given type.
     *
     * @param string $expected
     * @param mixed  $actual
     * @param string $message
     *
     * @since Method available since Release 3.5.0
     */
    public function assertNotInternalType(string $expected, mixed $actual, string $message = ''): void {
        if (!is_string($expected)) {
            throw InvalidArgumentHelperUtil::factory(1, 'string');
        }

        $constraint = new NotConstraint(
            new IsTypeConstraint($expected)
        );

        $this->assertThat($actual, $constraint, $message);

    }

    /**
     * Asserts that an attribute is of a given type.
     *
     * @param string        $expected
     * @param string        $attributeName
     * @param string|object $classOrObject
     * @param string        $message
     *
     * @since Method available since Release 3.5.0
     */
    public function assertAttributeNotInternalType(string $expected, string $attributeName, mixed $classOrObject, string $message = ''): void {

      $this->assertNotInternalType(
        $expected,
        $this->readAttribute($classOrObject, $attributeName),
        $message
      );

    }

    /**
     * Asserts that a string matches a given regular expression.
     *
     * @param string $pattern
     * @param string $string
     * @param string $message
     */
    public function assertRegExp(string $pattern, string $string, string $message = ''): void {

      $constraint = new PCREMatchConstraint($pattern);

      $this->assertThat($string, $constraint, $message);

    }

    /**
     * Asserts that a string does not match a given regular expression.
     *
     * @param string $pattern
     * @param string $string
     * @param string $message
     *
     * @since Method available since Release 2.1.0
     */
    public function assertNotRegExp(string $pattern, string $string, string $message = ''): void {

      $constraint = new NotConstraint(
        new PCREMatchConstraint($pattern)
      );

      $this->assertThat($string, $constraint, $message);

    }

    /**
     * Assert that the size of two arrays (or `Countable` or `Traversable` objects)
     * is the same.
     *
     * @param array|Countable|Traversable $expected
     * @param array|Countable|Traversable $actual
     * @param string                      $message
     */
    public function assertSameSize(mixed $expected, mixed $actual, string $message = ''): void {

      if (!$expected instanceof Countable &&
          !$expected instanceof Traversable &&
          !is_array($expected)) {
          throw InvalidArgumentHelperUtil::factory(1, 'countable or traversable');
      }

      if (!$actual instanceof Countable &&
          !$actual instanceof Traversable &&
          !is_array($actual)) {
          throw InvalidArgumentHelperUtil::factory(2, 'countable or traversable');
      }

      $this->assertThat(
          $actual,
          new SameSizeConstraint($expected),
          $message
      );

    }

    /**
     * Assert that the size of two arrays (or `Countable` or `Traversable` objects)
     * is not the same.
     *
     * @param array|Countable|Traversable $expected
     * @param array|Countable|Traversable $actual
     * @param string                      $message
     */
    public function assertNotSameSize(mixed $expected, mixed $actual, string $message = ''): void {
      if (!$expected instanceof Countable &&
        !$expected instanceof Traversable &&
        !is_array($expected)) {
        throw InvalidArgumentHelperUtil::factory(1, 'countable or traversable');
      }

      if (!$actual instanceof Countable &&
        !$actual instanceof Traversable &&
        !is_array($actual)) {
        throw InvalidArgumentHelperUtil::factory(2, 'countable or traversable');
      }

      $constraint = new NotConstraint(
        new SameSizeConstraint($expected)
      );

      $this->assertThat($actual, $constraint, $message);

    }

    /**
     * Asserts that a string matches a given format string.
     *
     * @param string $format
     * @param string $string
     * @param string $message
     *
     * @since Method available since Release 3.5.0
     */
    public function assertStringMatchesFormat(string $format, string $string, string $message = ''): void {

      $constraint = new StringMatchesConstraint($format);

      $this->assertThat($string, $constraint, $message);

    }

    /**
     * Asserts that a string does not match a given format string.
     *
     * @param string $format
     * @param string $string
     * @param string $message
     *
     * @since Method available since Release 3.5.0
     */
    public function assertStringNotMatchesFormat(string $format, string $string, string $message = ''): void {

      $constraint = new NotConstraint(
        new StringMatchesConstraint($format)
      );

      $this->assertThat($string, $constraint, $message);

    }

    /**
     * Asserts that a string matches a given format file.
     *
     * @param string $formatFile
     * @param string $string
     * @param string $message
     *
     * @since Method available since Release 3.5.0
     */
    public function assertStringMatchesFormatFile(string $formatFile, string $string, string $message = ''): void {

      $this->assertFileExists($formatFile, $message);

      $constraint = new StringMatchesConstraint(
        file_get_contents($formatFile)
      );

      $this->assertThat($string, $constraint, $message);

    }

    /**
     * Asserts that a string does not match a given format string.
     *
     * @param string $formatFile
     * @param string $string
     * @param string $message
     *
     * @since Method available since Release 3.5.0
     */
    public function assertStringNotMatchesFormatFile(string $formatFile, string $string, string $message = ''): void {

      $this->assertFileExists($formatFile, $message);

      $constraint = new NotConstraint(
        new StringMatchesConstraint(
          file_get_contents($formatFile)
        )
      );

      $this->assertThat($string, $constraint, $message);

    }

    /**
     * Asserts that a string starts with a given prefix.
     *
     * @param string $prefix
     * @param string $string
     * @param string $message
     *
     * @since Method available since Release 3.4.0
     */
    public function assertStringStartsWith(string $prefix, string $string, string $message = ''): void {

      $constraint = new StringStartsWithConstraint(
        $prefix
      );

      $this->assertThat($string, $constraint, $message);

    }

    /**
     * Asserts that a string starts not with a given prefix.
     *
     * @param string $prefix
     * @param string $string
     * @param string $message
     *
     * @since Method available since Release 3.4.0
     */
    public function assertStringStartsNotWith(string $prefix, string $string, string $message = ''): void {

      $constraint = new NotConstraint(
        new StringStartsWithConstraint($prefix)
      );

      $this->assertThat($string, $constraint, $message);

    }

    /**
     * Asserts that a string ends with a given suffix.
     *
     * @param string $suffix
     * @param string $string
     * @param string $message
     *
     * @since Method available since Release 3.4.0
     */
    public function assertStringEndsWith(string $suffix, string $string, string $message = ''): void {

      $constraint = new StringEndsWithConstraint($suffix);

      $this->assertThat($string, $constraint, $message);

    }

    /**
     * Asserts that a string ends not with a given suffix.
     *
     * @param string $suffix
     * @param string $string
     * @param string $message
     *
     * @since Method available since Release 3.4.0
     */
    public function assertStringEndsNotWith(string $suffix, string $string, string $message = ''): void {

      $constraint = new NotConstraint(
        new StringEndsWithConstraint($suffix)
      );

      $this->assertThat($string, $constraint, $message);

    }

    /**
     * Asserts that two XML files are equal.
     *
     * @param string $expectedFile
     * @param string $actualFile
     * @param string $message
     *
     * @since Method available since Release 3.1.0
     */
    public function assertXmlFileEqualsXmlFile(string $expectedFile, string $actualFile, string $message = ''): void {

      $expected = XMLUtil::loadFile($expectedFile);
      $actual   = XMLUtil::loadFile($actualFile);

      $this->assertEquals($expected, $actual, $message);

    }

    /**
     * Asserts that two XML files are not equal.
     *
     * @param string $expectedFile
     * @param string $actualFile
     * @param string $message
     *
     * @since Method available since Release 3.1.0
     */
    public function assertXmlFileNotEqualsXmlFile(string $expectedFile, string $actualFile, string $message = ''): void {

      $expected = XMLUtil::loadFile($expectedFile);
      $actual   = XMLUtil::loadFile($actualFile);

      $this->assertNotEquals($expected, $actual, $message);

    }

    /**
     * Asserts that two XML documents are equal.
     *
     * @param string $expectedFile
     * @param string $actualXml
     * @param string $message
     *
     * @since Method available since Release 3.3.0
     */
    public function assertXmlStringEqualsXmlFile(string $expectedFile, string $actualXml, string $message = ''): void {

      $expected = XMLUtil::loadFile($expectedFile);
      $actual   = XMLUtil::load($actualXml);

      $this->assertEquals($expected, $actual, $message);

    }

    /**
     * Asserts that two XML documents are not equal.
     *
     * @param string $expectedFile
     * @param string $actualXml
     * @param string $message
     *
     * @since Method available since Release 3.3.0
     */
    public function assertXmlStringNotEqualsXmlFile(string $expectedFile, string $actualXml, string $message = ''): void {

      $expected = XMLUtil::loadFile($expectedFile);
      $actual   = XMLUtil::load($actualXml);

      $this->assertNotEquals($expected, $actual, $message);

    }

    /**
     * Asserts that two XML documents are equal.
     *
     * @param string $expectedXml
     * @param string $actualXml
     * @param string $message
     *
     * @since Method available since Release 3.1.0
     */
    public function assertXmlStringEqualsXmlString(string $expectedXml, string $actualXml, string $message = ''): void {

      $expected = XMLUtil::load($expectedXml);
      $actual   = XMLUtil::load($actualXml);

      $this->assertEquals($expected, $actual, $message);

    }

    /**
     * Asserts that two XML documents are not equal.
     *
     * @param string $expectedXml
     * @param string $actualXml
     * @param string $message
     *
     * @since Method available since Release 3.1.0
     */
    public function assertXmlStringNotEqualsXmlString(string $expectedXml, string $actualXml, string $message = ''): void {

      $expected = XMLUtil::load($expectedXml);
      $actual   = XMLUtil::load($actualXml);

      $this->assertNotEquals($expected, $actual, $message);

    }

    /**
     * Asserts that a hierarchy of DOMElements matches.
     *
     * @param DOMElement $expectedElement
     * @param DOMElement $actualElement
     * @param bool       $checkAttributes
     * @param string     $message
     *
     * @since Method available since Release 3.3.0
     */
    public function assertEqualXMLStructure(DOMElement $expectedElement, DOMElement $actualElement, bool $checkAttributes = false, string $message = ''): void {

      $tmp             = new DOMDocument();
      $expectedElement = $tmp->importNode($expectedElement, true);

      $tmp           = new DOMDocument();
      $actualElement = $tmp->importNode($actualElement, true);

      $this->assertEquals(
        $expectedElement->tagName,
        $actualElement->tagName,
        $message
      );

      if ($checkAttributes) {

        $this->assertEquals(
          $expectedElement->attributes->length,
          $actualElement->attributes->length,
          sprintf(
            '%s%sNumber of attributes on node "%s" does not match',
            $message,
            $message != '' ? "\n" : '',
            $expectedElement->tagName
          )
        );

        for ($i = 0; $i < $expectedElement->attributes->length; $i++) {

          $expectedAttribute = $expectedElement->attributes->item($i);

          $actualAttribute   = $actualElement->attributes->getNamedItem(
            $expectedAttribute->name
          );

          if (!$actualAttribute) {
            $this->fail(
              sprintf(
                '%s%sCould not find attribute "%s" on node "%s"',
                $message,
                $message != '' ? "\n" : '',
                $expectedAttribute->name,
                $expectedElement->tagName
              )
            );
          }
        }

      }

      XMLUtil::removeCharacterDataNodes($expectedElement);
      XMLUtil::removeCharacterDataNodes($actualElement);

      $this->assertEquals(
        $expectedElement->childNodes->length,
        $actualElement->childNodes->length,
        sprintf(
          '%s%sNumber of child nodes of "%s" differs',
          $message,
          $message != '' ? "\n" : '',
          $expectedElement->tagName
        )
      );

      for ($i = 0; $i < $expectedElement->childNodes->length; $i++) {
        $this->assertEqualXMLStructure(
          $expectedElement->childNodes->item($i),
          $actualElement->childNodes->item($i),
          $checkAttributes,
          $message
        );
      }

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
    public function assertThat(mixed $value, ConstraintInterface $constraint, string $message = ''): void {
      $this->count += count($constraint);
      $constraint->evaluate($value, $message);
    }

    /**
     * Asserts that a string is a valid JSON string.
     *
     * @param string $actualJson
     * @param string $message
     *
     * @since Method available since Release 3.7.20
     */
    public function assertJson(string $actualJson, string $message = ''): void {
      $this->assertThat($actualJson, $this->isJson(), $message);
    }

    /**
     * Asserts that two given JSON encoded objects or arrays are equal.
     *
     * @param string $expectedJson
     * @param string $actualJson
     * @param string $message
     */
    public function assertJsonStringEqualsJsonString(string $expectedJson, string $actualJson, string $message = ''): void {

      $this->assertJson($expectedJson, $message);
      $this->assertJson($actualJson, $message);

      $expected = json_decode($expectedJson);
      $actual   = json_decode($actualJson);

      $this->assertEquals($expected, $actual, $message);

    }

    /**
     * Asserts that two given JSON encoded objects or arrays are not equal.
     *
     * @param string $expectedJson
     * @param string $actualJson
     * @param string $message
     */
    public function assertJsonStringNotEqualsJsonString(string $expectedJson, string $actualJson, string $message = ''): void {

      $this->assertJson($expectedJson, $message);
      $this->assertJson($actualJson, $message);

      $expected = json_decode($expectedJson);
      $actual   = json_decode($actualJson);

      $this->assertNotEquals($expected, $actual, $message);

    }

    /**
     * Asserts that the generated JSON encoded object and the content of the given file are equal.
     *
     * @param string $expectedFile
     * @param string $actualJson
     * @param string $message
     */
    public function assertJsonStringEqualsJsonFile(string $expectedFile, string $actualJson, string $message = ''): void {

      $this->assertFileExists($expectedFile, $message);
      $expectedJson = file_get_contents($expectedFile);

      $this->assertJson($expectedJson, $message);
      $this->assertJson($actualJson, $message);

      // call constraint
      $constraint = new JsonMatchesConstraint(
        $expectedJson
      );

      $this->assertThat($actualJson, $constraint, $message);

    }

    /**
     * Asserts that the generated JSON encoded object and the content of the given file are not equal.
     *
     * @param string $expectedFile
     * @param string $actualJson
     * @param string $message
     */
    public function assertJsonStringNotEqualsJsonFile(string $expectedFile, string $actualJson, string $message = ''): void {
      $this->assertFileExists($expectedFile, $message);
      $expectedJson = file_get_contents($expectedFile);

      $this->assertJson($expectedJson, $message);
      $this->assertJson($actualJson, $message);

      // call constraint
      $constraint = new JsonMatchesConstraint(
        $expectedJson
      );

      $this->assertThat($actualJson, new NotConstraint($constraint), $message);

    }

    /**
     * Asserts that two JSON files are equal.
     *
     * @param string $expectedFile
     * @param string $actualFile
     * @param string $message
     */
    public function assertJsonFileEqualsJsonFile(string $expectedFile, string $actualFile, string $message = ''): void {

      $this->assertFileExists($expectedFile, $message);
      $this->assertFileExists($actualFile, $message);

      $actualJson   = file_get_contents($actualFile);
      $expectedJson = file_get_contents($expectedFile);

      $this->assertJson($expectedJson, $message);
      $this->assertJson($actualJson, $message);

      // call constraint
      $constraintExpected = new JsonMatchesConstraint(
        $expectedJson
      );

      $constraintActual = new JsonMatchesConstraint($actualJson);

      $this->assertThat($expectedJson, $constraintActual, $message);
      $this->assertThat($actualJson, $constraintExpected, $message);

    }

    /**
     * Asserts that two JSON files are not equal.
     *
     * @param string $expectedFile
     * @param string $actualFile
     * @param string $message
     */
    public function assertJsonFileNotEqualsJsonFile(string $expectedFile, string $actualFile, string $message = ''): void {
      $this->assertFileExists($expectedFile, $message);
      $this->assertFileExists($actualFile, $message);

      $actualJson   = file_get_contents($actualFile);
      $expectedJson = file_get_contents($expectedFile);

      $this->assertJson($expectedJson, $message);
      $this->assertJson($actualJson, $message);

      // call constraint
      $constraintExpected = new JsonMatchesConstraint(
        $expectedJson
      );

      $constraintActual = new JsonMatchesConstraint($actualJson);

      $this->assertThat($expectedJson, new NotConstraint($constraintActual), $message);
      $this->assertThat($actualJson, new NotConstraint($constraintExpected), $message);

    }

    /**
     * Returns a AndConstraint matcher object.
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.0.0
     */
    public function logicalAnd(Vector<ConstraintInterface> $constraints): ConstraintInterface {
      $constraint = new AndConstraint();
      $constraint->setConstraints($constraints);
      return $constraint;
    }

    /**
     * Returns a PHPUnit_Framework_Constraint_Or matcher object.
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.0.0
     */
    public function logicalOr(mixed ... $args): ConstraintInterface {

      $t_constraints = func_get_args();

      $constraintVector = Vector {};
      foreach ( $t_constraints as $t_constraint ) {
        if ( $t_constraint instanceof ConstraintInterface ) {
          $constraintVector->add($t_constraint);
        }
      }

      $constraint = new OrConstraint();
      $constraint->setConstraints($constraintVector);

      return $constraint;

    }

    /**
     * Returns a PHPUnit_Framework_Constraint_Not matcher object.
     *
     * @param PHPUnit_Framework_Constraint $constraint
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.0.0
     */
    public function logicalNot(ConstraintInterface $constraint): ConstraintInterface
    {
        return new NotConstraint($constraint);
    }

    /**
     * Returns a XorConstraint matcher object.
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.0.0
     */
    public function logicalXor(mixed...$args): ConstraintInterface {

      $constraints = Vector {};

      foreach ( $args as $t_constraint) {
        if ( $t_constraint instanceof ConstraintInterface ) {
          $constraints->add($t_constraint);
        }
      }

      $constraint = new XorConstraint();
      $constraint->setConstraints($constraints);

      return $constraint;

    }

    /**
     * Returns a IsAnythingConstraint matcher object.
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.0.0
     */
    public function anything(): ConstraintInterface {
        return new IsAnythingConstraint();
    }

    /**
     * Returns a IsTrueConstraint matcher object.
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.3.0
     */
    public function isTrue(): ConstraintInterface {
      return new IsTrueConstraint();
    }

    /**
     * Returns a CallbackConstraint matcher object.
     *
     * @param callable $callback
     *
     * @return ConstraintInterface
     */
    public function callback(mixed $callback): ConstraintInterface {
        return new CallbackConstraint($callback);
    }

    /**
     * Returns a IsFalseConstraint matcher object.
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.3.0
     */
    public function isFalse(): ConstraintInterface {
        return new IsFalseConstraint();
    }

    /**
     * Returns a IsJsonConstraint matcher object.
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.7.20
     */
    public function isJson(): ConstraintInterface {
      return new IsJsonConstraint();
    }

    /**
     * Returns a IsNullConstraint matcher object.
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.3.0
     */
    public function isNull(): ConstraintInterface {
      return new IsNullConstraint();
    }

    /**
     * Returns a IsFiniteConstraint matcher object.
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 5.0.0
     */
    public function isFinite(): ConstraintInterface {
      return new IsFiniteConstraint();
    }

    /**
     * Returns a IsInfiniteConstraint matcher object.
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 5.0.0
     */
    public function isInfinite(): ConstraintInterface {
      return new IsInfiniteConstraint();
    }

    /**
     * Returns a IsNanConstraint matcher object.
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 5.0.0
     */
    public function isNan(): ConstraintInterface {
        return new IsNanConstraint();
    }

    /**
     * Returns a PHPUnit_Framework_Constraint_Attribute matcher object.
     *
     * @param PHPUnit_Framework_Constraint $constraint
     * @param string                       $attributeName
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.1.0
     */
    public function attribute(ConstraintInterface $constraint, string $attributeName): ConstraintInterface {
      return new AttributeConstraint(
        $constraint,
        $attributeName,
        $this
      );
    }

    /**
     * Returns a TraversableContainsConstraint matcher
     * object.
     *
     * @param mixed $value
     * @param bool  $checkForObjectIdentity
     * @param bool  $checkForNonObjectIdentity
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.0.0
     */
    public function contains(mixed $value, bool $checkForObjectIdentity = true, bool $checkForNonObjectIdentity = false): ConstraintInterface
    {
        return new TraversableContainsConstraint($value, $checkForObjectIdentity, $checkForNonObjectIdentity);
    }

    /**
     * Returns a TraversableContainsOnlyConstraint matcher
     * object.
     *
     * @param string $type
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.1.4
     */
    public function containsOnly(mixed $type): ConstraintInterface {
      return new TraversableContainsOnlyConstraint($type);
    }

    /**
     * Returns a TraversableContainsOnlyConstraint matcher
     * object.
     *
     * @param string $classname
     *
     * @return ConstraintInterface
     */
    public function containsOnlyInstancesOf(mixed $classname): ConstraintInterface
    {
        return new TraversableContainsOnlyConstraint($classname, false);
    }

    /**
     * Returns a PHPUnit_Framework_Constraint_ArrayHasKey matcher object.
     *
     * @param mixed $key
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.0.0
     */
    public function arrayHasKey(mixed $key): ConstraintInterface {
      return new ArrayHasKeyConstraint($key);
    }

    /**
     * Returns a PHPUnit_Framework_Constraint_IsEqual matcher object.
     *
     * @param mixed $value
     * @param float $delta
     * @param int   $maxDepth
     * @param bool  $canonicalize
     * @param bool  $ignoreCase
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.0.0
     */
    public function equalTo(mixed $value, float $delta = 0.0, int $maxDepth = 10, bool $canonicalize = false, bool $ignoreCase = false): ConstraintInterface
    {
        return new IsEqualConstraint(
            $value,
            $delta,
            $maxDepth,
            $canonicalize,
            $ignoreCase
        );
    }

    /**
     * Returns a PHPUnit_Framework_Constraint_IsEqual matcher object
     * that is wrapped in a PHPUnit_Framework_Constraint_Attribute matcher
     * object.
     *
     * @param string $attributeName
     * @param mixed  $value
     * @param float  $delta
     * @param int    $maxDepth
     * @param bool   $canonicalize
     * @param bool   $ignoreCase
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.1.0
     */
    public function attributeEqualTo(string $attributeName, mixed $value, float $delta = 0.0, int $maxDepth = 10, bool $canonicalize = false, bool $ignoreCase = false): ConstraintInterface {
        return $this->attribute(
            $this->equalTo(
                $value,
                $delta,
                $maxDepth,
                $canonicalize,
                $ignoreCase
            ),
            $attributeName
        );
    }

    /**
     * Returns a IsEmptyConstraint matcher object.
     *
     * @return ConstraintInteface
     *
     * @since Method available since Release 3.5.0
     */
    public function isEmpty(): ConstraintInterface {
        return new IsEmptyConstraint();
    }

    /**
     * Returns a PHPUnit_Framework_Constraint_FileExists matcher object.
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.0.0
     */
    public function fileExists(): ConstraintInterface {
      return new FileExistsConstraint();
    }

    /**
     * Returns a PHPUnit_Framework_Constraint_GreaterThan matcher object.
     *
     * @param mixed $value
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.0.0
     */
    public function greaterThan(mixed $value): ConstraintInterface {
      return new GreaterThanConstraint($value);
    }

    /**
     * Returns a PHPUnit_Framework_Constraint_Or matcher object that wraps
     * a PHPUnit_Framework_Constraint_IsEqual and a
     * PHPUnit_Framework_Constraint_GreaterThan matcher object.
     *
     * @param mixed $value
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.1.0
     */
    public function greaterThanOrEqual(mixed $value): ConstraintInterface {
      return $this->logicalOr(
          new IsEqualConstraint($value),
          new GreaterThanConstraint($value)
      );
    }

    /**
     * Returns a PHPUnit_Framework_Constraint_ClassHasAttribute matcher object.
     *
     * @param string $attributeName
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.1.0
     */
    public function classHasAttribute(mixed $attributeName): ConstraintInterface {
      return new ClassHasAttributeConstraint(
        $attributeName
      );
    }

    /**
     * Returns a PHPUnit_Framework_Constraint_ClassHasStaticAttribute matcher
     * object.
     *
     * @param string $attributeName
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.1.0
     */
    public function classHasStaticAttribute(mixed $attributeName): ConstraintInterface {
      return new ClassHasStaticAttributeConstraint(
        $attributeName
      );
    }

    /**
     * Returns a ObjectHasAttributeConstraint matcher object.
     *
     * @param string $attributeName
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.0.0
     */
    public function objectHasAttribute(mixed $attributeName): ConstraintInterface {
      return new ObjectHasAttributeConstraint(
        $attributeName
      );
    }

    /**
     * Returns a IsIdenticalConstraint matcher object.
     *
     * @param mixed $value
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.0.0
     */
    public function identicalTo(mixed $value): ConstraintInterface {
      return new IsIdenticalConstraint($value);
    }

    /**
     * Returns a PHPUnit_Framework_Constraint_IsInstanceOf matcher object.
     *
     * @param string $className
     *
     * @return ConstraintInteface
     *
     * @since Method available since Release 3.0.0
     */
    public function isInstanceOf(string $className): ConstraintInterface {
      return new IsInstanceOfConstraint($className);
    }

    /**
     * Returns a PHPUnit_Framework_Constraint_IsType matcher object.
     *
     * @param string $type
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.0.0
     */
    public function isType(mixed $type): ConstraintInterface {
      return new IsTypeConstraint($type);
    }

    /**
     * Returns a PHPUnit_Framework_Constraint_LessThan matcher object.
     *
     * @param mixed $value
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.0.0
     */
    public function lessThan(mixed $value): ConstraintInterface {
      return new LessThanConstraint($value);
    }

    /**
     * Returns a PHPUnit_Framework_Constraint_Or matcher object that wraps
     * a PHPUnit_Framework_Constraint_IsEqual and a
     * PHPUnit_Framework_Constraint_LessThan matcher object.
     *
     * @param mixed $value
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.1.0
     */
    public function lessThanOrEqual(mixed $value): ConstraintInterface {
        return $this->logicalOr(
            new IsEqualConstraint($value),
            new LessThanConstraint($value)
        );
    }

    /**
     * Returns a PCREMatchConstraint matcher object.
     *
     * @param string $pattern
     *
     * @return ConstraintInteface
     *
     * @since Method available since Release 3.0.0
     */
    public function matchesRegularExpression(mixed $pattern): ConstraintInterface {
      return new PCREMatchConstraint($pattern);
    }

    /**
     * Returns a StringMatchesConstraint matcher object.
     *
     * @param string $string
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.5.0
     */
    public function matches(mixed $string): ConstraintInterface {
      return new StringMatchesConstraint($string);
    }

    /**
     * Returns a PHPUnit_Framework_Constraint_StringStartsWith matcher object.
     *
     * @param mixed $prefix
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.4.0
     */
    public function stringStartsWith(mixed $prefix): ConstraintInterface {
      return new StringStartsWithConstraint($prefix);
    }

    /**
     * Returns a PHPUnit_Framework_Constraint_StringContains matcher object.
     *
     * @param string $string
     * @param bool   $case
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.0.0
     */
    public function stringContains(string $string, bool $case = true): ConstraintInterface
    {
        return new StringContainsConstraint($string, $case);
    }

    /**
     * Returns a PHPUnit_Framework_Constraint_StringEndsWith matcher object.
     *
     * @param mixed $suffix
     *
     * @return ConstraintInterface
     *
     * @since Method available since Release 3.4.0
     */
    public function stringEndsWith(mixed $suffix): ConstraintInterface {
      return new StringEndsWithConstraint($suffix);
    }

    /**
     * Returns a PHPUnit_Framework_Constraint_Count matcher object.
     *
     * @param int $count
     *
     * @return ConstraintInterface
     */
    public function countOf(mixed $count): ConstraintInterface {
      return new CountConstraint($count);
    }

    /**
     * Fails a test with the given message.
     *
     * @param string $message
     *
     * @throws AssertionFailedError
     */
    public function fail(string $message = ''): void {
      throw new AssertionFailedError($message);
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
     * @throws PHPUnit_Exceptions_Exception
     */
    public function readAttribute(mixed $classOrObject, string $attributeName): mixed
    {
        if (!is_string($attributeName)) {
            throw InvalidArgumentHelperUtil::factory(2, 'string');
        }

        if (!preg_match('/[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*/', $attributeName)) {
            throw InvalidArgumentHelperUtil::factory(2, 'valid attribute name');
        }

        if (is_string($classOrObject)) {
            if (!class_exists($classOrObject)) {
                throw InvalidArgumentHelperUtil::factory(
                    1,
                    'class name'
                );
            }

            return $this->getStaticAttribute(
                $classOrObject,
                $attributeName
            );
        } elseif (is_object($classOrObject)) {
            return $this->getObjectAttribute(
                $classOrObject,
                $attributeName
            );
        } else {
            throw InvalidArgumentHelperUtil::factory(
                1,
                'class name or object'
            );
        }
    }

    /**
     * Returns the value of a attribute.
     * This also works for attributes that are declared protected or private.
     *
     * @param string $className
     * @param string $attributeName
     *
     * @return mixed
     *
     * @throws PHPUnit_Exceptions_Exception
     *
     * @since Method available since Release 4.0.0
     */
    public function getStaticAttribute(string $className, string $attributeName): mixed {

      if (!is_string($className)) {
        throw InvalidArgumentHelperUtil::factory(1, 'string');
      }

      if (!class_exists($className)) {
        throw InvalidArgumentHelperUtil::factory(1, 'class name');
      }

      if (!is_string($attributeName)) {
        throw InvalidArgumentHelperUtil::factory(2, 'string');
      }

      if (!preg_match('/[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*/', $attributeName)) {
        throw InvalidArgumentHelperUtil::factory(2, 'valid attribute name');
      }

      $class = new ReflectionClass($className);

      while ($class instanceof ReflectionClass) {

        $attributes = $class->getStaticProperties();

        if (array_key_exists($attributeName, $attributes)) {
          return $attributes[$attributeName];
        }

        $class = $class->getParentClass();

      }

      throw new PHPUnit_Exceptions_Exception(
        sprintf(
          'Attribute "%s" not found in class.',
          $attributeName
        )
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
     * @throws PHPUnit_Exceptions_Exception
     *
     * @since Method available since Release 4.0.0
     */
    public function getObjectAttribute(mixed $object, string $attributeName): mixed {
      if (!is_object($object)) {
        throw InvalidArgumentHelperUtil::factory(1, 'object');
      }

      if (!is_string($attributeName)) {
        throw InvalidArgumentHelperUtil::factory(2, 'string');
      }

      if (!preg_match('/[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*/', $attributeName)) {
        throw InvalidArgumentHelperUtil::factory(2, 'valid attribute name');
      }

      $attribute = null;

      try {
          $attribute = new ReflectionProperty($object, $attributeName);
      } catch (ReflectionException $e) {

          $reflector = new ReflectionObject($object);

          while ($reflector instanceof ReflectionObject && $reflector = $reflector->getParentClass()) {
            try {
              $attribute = $reflector->getProperty($attributeName);
              break;
            } catch (ReflectionException $e) {
          }
        }
      }

      if ($attribute instanceof ReflectionProperty) {

        $attribute->setAccessible(true);
        $value = $attribute->getValue($object);
        $attribute->setAccessible(false);

        return $value;

      }

      throw new PHPUnit_Exceptions_Exception(
        sprintf(
          'Attribute "%s" not found in object.',
          $attributeName
        )
      );
    }

    /**
     * Mark the test as incomplete.
     *
     * @param string $message
     *
     * @throws IncompleteTestError
     *
     * @since Method available since Release 3.0.0
     */
    public function markTestIncomplete(string $message = ''): void {
      throw new IncompleteTestError($message, 0,'unknown', 0);
    }

    /**
     * Mark the test as skipped.
     *
     * @param string $message
     *
     * @throws SkippedTestError
     *
     * @since Method available since Release 3.0.0
     */
    public function markTestSkipped(string $message = ''): void {
      throw new SkippedTestError($message);
    }

    /**
     * Return the current assertion count.
     *
     * @return int
     *
     * @since Method available since Release 3.3.3
     */
    public function getCount(): int {
      return $this->count;
    }

    /**
     * Reset the assertion counter.
     *
     * @since Method available since Release 3.3.3
     */
    public function resetCount(): bool {
      $this->count = 0;
      return true;
    }
}
