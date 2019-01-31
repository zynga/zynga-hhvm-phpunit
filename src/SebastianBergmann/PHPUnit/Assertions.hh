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
use SebastianBergmann\PHPUnit\Assertions\AssertAttributeContainsOnly;
use SebastianBergmann\PHPUnit\Assertions\AssertAttributeCount;
use SebastianBergmann\PHPUnit\Assertions\AssertAttributeEmpty;
use SebastianBergmann\PHPUnit\Assertions\AssertAttributeEquals;
use SebastianBergmann\PHPUnit\Assertions\AssertAttributeGreaterThan;
use SebastianBergmann\PHPUnit\Assertions\AssertAttributeGreaterThanOrEqual;
use SebastianBergmann\PHPUnit\Assertions\AssertAttributeInstanceOf;
use SebastianBergmann\PHPUnit\Assertions\AssertAttributeInternalType;
use SebastianBergmann\PHPUnit\Assertions\AssertAttributeLessThan;
use SebastianBergmann\PHPUnit\Assertions\AssertAttributeLessThanOrEqual;
use SebastianBergmann\PHPUnit\Assertions\AssertAttributeNotContains;
use SebastianBergmann\PHPUnit\Assertions\AssertAttributeNotContainsOnly;
use SebastianBergmann\PHPUnit\Assertions\AssertAttributeNotCount;
use SebastianBergmann\PHPUnit\Assertions\AssertAttributeNotEmpty;
use SebastianBergmann\PHPUnit\Assertions\AssertAttributeNotEquals;
use SebastianBergmann\PHPUnit\Assertions\AssertAttributeNotInstanceOf;
use SebastianBergmann\PHPUnit\Assertions\AssertAttributeNotInternalType;
use SebastianBergmann\PHPUnit\Assertions\AssertAttributeNotSame;
use SebastianBergmann\PHPUnit\Assertions\AssertAttributeSame;
use SebastianBergmann\PHPUnit\Assertions\AssertClassHasAttribute;
use SebastianBergmann\PHPUnit\Assertions\AssertClassHasStaticAttribute;
use SebastianBergmann\PHPUnit\Assertions\AssertClassNotHasAttribute;
use SebastianBergmann\PHPUnit\Assertions\AssertClassNotHasStaticAttribute;
use SebastianBergmann\PHPUnit\Assertions\AssertContains;
use SebastianBergmann\PHPUnit\Assertions\AssertContainsOnly;
use SebastianBergmann\PHPUnit\Assertions\AssertContainsOnlyInstancesOf;
use SebastianBergmann\PHPUnit\Assertions\AssertCount;
use SebastianBergmann\PHPUnit\Assertions\AssertEmpty;
use SebastianBergmann\PHPUnit\Assertions\AssertEquals;
use SebastianBergmann\PHPUnit\Assertions\AssertFileEquals;
use SebastianBergmann\PHPUnit\Assertions\AssertFileExists;
use SebastianBergmann\PHPUnit\Assertions\AssertFileNotEquals;
use SebastianBergmann\PHPUnit\Assertions\AssertFileNotExists;
use SebastianBergmann\PHPUnit\Assertions\AssertFinite;
use SebastianBergmann\PHPUnit\Assertions\AssertGreaterThan;
use SebastianBergmann\PHPUnit\Assertions\AssertGreaterThanOrEqual;
use SebastianBergmann\PHPUnit\Assertions\AssertInfinite;
use SebastianBergmann\PHPUnit\Assertions\AssertInstanceOf;
use SebastianBergmann\PHPUnit\Assertions\AssertInternalType;
use SebastianBergmann\PHPUnit\Assertions\AssertLessThan;
use SebastianBergmann\PHPUnit\Assertions\AssertLessThanOrEqual;
use SebastianBergmann\PHPUnit\Assertions\AssertNan;
use SebastianBergmann\PHPUnit\Assertions\AssertNotContainsOnly;
use SebastianBergmann\PHPUnit\Assertions\AssertNotContains;
use SebastianBergmann\PHPUnit\Assertions\AssertNotCount;
use SebastianBergmann\PHPUnit\Assertions\AssertNotEmpty;
use SebastianBergmann\PHPUnit\Assertions\AssertNotEquals;
use SebastianBergmann\PHPUnit\Assertions\AssertNotInstanceOf;
use SebastianBergmann\PHPUnit\Assertions\AssertNotInternalType;
use SebastianBergmann\PHPUnit\Assertions\AssertNotNull;
use SebastianBergmann\PHPUnit\Assertions\AssertNotRegExp;
use SebastianBergmann\PHPUnit\Assertions\AssertNotSame;
use SebastianBergmann\PHPUnit\Assertions\AssertNotSameSize;
use SebastianBergmann\PHPUnit\Assertions\AssertNotTrue;
use SebastianBergmann\PHPUnit\Assertions\AssertNull;
use SebastianBergmann\PHPUnit\Assertions\AssertObjectHasAttribute;
use SebastianBergmann\PHPUnit\Assertions\AssertObjectNotHasAttribute;
use SebastianBergmann\PHPUnit\Assertions\AssertRegExp;
use SebastianBergmann\PHPUnit\Assertions\AssertSame;
use SebastianBergmann\PHPUnit\Assertions\AssertSameSize;
use SebastianBergmann\PHPUnit\Assertions\AssertStringEqualsFile;
use SebastianBergmann\PHPUnit\Assertions\AssertStringMatchesFormat;
use SebastianBergmann\PHPUnit\Assertions\AssertStringMatchesFormatFile;
use SebastianBergmann\PHPUnit\Assertions\AssertStringNotEqualsFile;
use SebastianBergmann\PHPUnit\Assertions\AssertStringNotMatchesFormat;
use SebastianBergmann\PHPUnit\Assertions\AssertThat;
use SebastianBergmann\PHPUnit\Assertions\AssertTrue;
use SebastianBergmann\PHPUnit\Assertions\GetObjectAttribute;
use SebastianBergmann\PHPUnit\Assertions\GetStaticAttribute;
use SebastianBergmann\PHPUnit\Assertions\ReadAttribute;
use SebastianBergmann\PHPUnit\Interfaces\ConstraintInterface;
use SebastianBergmann\PHPUnit\Exceptions\AssertionFailedException;

class Assertions {

  private static ?AssertionsCount $count;

  final public function counter(): AssertionsCount {
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
   * Asserts the number of elements of an array, Countable or Traversable.
   *
   * @param int    $expectedCount
   * @param mixed  $haystack
   * @param string $message
   */
  final public function assertCount(
    int $expectedCount,
    mixed $haystack,
    string $message = '',
  ): bool {

    return AssertCount::evaluate($this, $expectedCount, $haystack, $message);

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
  final public function assertAttributeCount(
    int $expectedCount,
    string $haystackAttributeName,
    mixed $haystackClassOrObject,
    string $message = '',
  ): bool {

    return AssertAttributeCount::evaluate(
      $this,
      $expectedCount,
      $haystackAttributeName,
      $haystackClassOrObject,
      $message,
    );

  }

  /**
   * Asserts the number of elements of an array, Countable or Traversable.
   *
   * @param int    $expectedCount
   * @param mixed  $haystack
   * @param string $message
   */
  final public function assertNotCount(
    int $expectedCount,
    mixed $haystack,
    string $message = '',
  ): bool {

    return
      AssertNotCount::evaluate($this, $expectedCount, $haystack, $message);

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
  final public function assertAttributeNotCount(
    int $expectedCount,
    string $haystackAttributeName,
    mixed $haystackClassOrObject,
    string $message = '',
  ): bool {

    return AssertAttributeNotCount::evaluate(
      $this,
      $expectedCount,
      $haystackAttributeName,
      $haystackClassOrObject,
      $message,
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
  final public function assertEquals(
    mixed $expected,
    mixed $actual,
    string $message = '',
    float $delta = 0.0,
    int $maxDepth = 10,
    bool $canonicalize = false,
    bool $ignoreCase = false,
  ): bool {

    return AssertEquals::evaluate(
      $this,
      $expected,
      $actual,
      $message,
      $delta,
      $maxDepth,
      $canonicalize,
      $ignoreCase,
    );

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
  final public function assertAttributeEquals(
    mixed $expected,
    string $actualAttributeName,
    mixed $actualClassOrObject,
    string $message = '',
    float $delta = 0.0,
    int $maxDepth = 10,
    bool $canonicalize = false,
    bool $ignoreCase = false,
  ): bool {

    return AssertAttributeEquals::evaluate(
      $this,
      $expected,
      $actualAttributeName,
      $actualClassOrObject,
      $message,
      $delta,
      $maxDepth,
      $canonicalize,
      $ignoreCase,
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
  final public function assertNotEquals(
    mixed $expected,
    mixed $actual,
    string $message = '',
    float $delta = 0.0,
    int $maxDepth = 10,
    bool $canonicalize = false,
    bool $ignoreCase = false,
  ): bool {

    return AssertNotEquals::evaluate(
      $this,
      $expected,
      $actual,
      $message,
      $delta,
      $maxDepth,
      $canonicalize,
      $ignoreCase,
    );

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
  final public function assertAttributeNotEquals(
    mixed $expected,
    string $actualAttributeName,
    mixed $actualClassOrObject,
    string $message = '',
    float $delta = 0.0,
    int $maxDepth = 10,
    bool $canonicalize = false,
    bool $ignoreCase = false,
  ): bool {

    return AssertAttributeNotEquals::evaluate(
      $this,
      $expected,
      $actualAttributeName,
      $actualClassOrObject,
      $message,
      $delta,
      $maxDepth,
      $canonicalize,
      $ignoreCase,
    );

  }

  /**
   * Asserts that a variable is empty.
   *
   * @param mixed  $actual
   * @param string $message
   *
   * @throws PHPUnit_Framework_AssertionFailedError
   */
  final public function assertEmpty(
    mixed $actual,
    string $message = '',
  ): bool {

    return AssertEmpty::evaluate($this, $actual, $message);

  }

  /**
   * Asserts that a static attribute of a class or an attribute of an object
   * is empty.
   *
   * @param string        $haystackAttributeName
   * @param string|object $haystackClassOrObject
   * @param string        $message
   *
   * @since Method available since Release 3.5.0
   */
  final public function assertAttributeEmpty(
    string $haystackAttributeName,
    mixed $haystackClassOrObject,
    string $message = '',
  ): bool {

    return AssertAttributeEmpty::evaluate(
      $this,
      $haystackAttributeName,
      $haystackClassOrObject,
      $message,
    );

  }

  /**
   * Asserts that a variable is not empty.
   *
   * @param mixed  $actual
   * @param string $message
   *
   * @throws PHPUnit_Framework_AssertionFailedError
   */
  final public function assertNotEmpty(
    mixed $actual,
    string $message = '',
  ): bool {

    return AssertNotEmpty::evaluate($this, $actual, $message);

  }

  /**
   * Asserts that a static attribute of a class or an attribute of an object
   * is not empty.
   *
   * @param string        $haystackAttributeName
   * @param string|object $haystackClassOrObject
   * @param string        $message
   *
   * @since Method available since Release 3.5.0
   */
  final public function assertAttributeNotEmpty(
    string $haystackAttributeName,
    mixed $haystackClassOrObject,
    string $message = '',
  ): bool {

    return AssertAttributeNotEmpty::evaluate(
      $this,
      $haystackAttributeName,
      $haystackClassOrObject,
      $message,
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
  final public function assertGreaterThan(
    mixed $expected,
    mixed $actual,
    string $message = '',
  ): bool {

    return AssertGreaterThan::evaluate($this, $expected, $actual, $message);

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
  final public function assertAttributeGreaterThan(
    mixed $expected,
    string $actualAttributeName,
    mixed $actualClassOrObject,
    string $message = '',
  ): bool {

    return AssertAttributeGreaterThan::evaluate(
      $this,
      $expected,
      $actualAttributeName,
      $actualClassOrObject,
      $message,
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
  final public function assertGreaterThanOrEqual(
    mixed $expected,
    mixed $actual,
    string $message = '',
  ): bool {

    return
      AssertGreaterThanOrEqual::evaluate($this, $expected, $actual, $message);

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
  final public function assertAttributeGreaterThanOrEqual(
    mixed $expected,
    string $actualAttributeName,
    mixed $actualClassOrObject,
    string $message = '',
  ): bool {

    return AssertAttributeGreaterThanOrEqual::evaluate(
      $this,
      $expected,
      $actualAttributeName,
      $actualClassOrObject,
      $message,
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
  final public function assertLessThan(
    mixed $expected,
    mixed $actual,
    string $message = '',
  ): bool {

    return AssertLessThan::evaluate($this, $expected, $actual, $message);

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
  final public function assertAttributeLessThan(
    mixed $expected,
    string $actualAttributeName,
    mixed $actualClassOrObject,
    string $message = '',
  ): bool {

    return AssertAttributeLessThan::evaluate(
      $this,
      $expected,
      $actualAttributeName,
      $actualClassOrObject,
      $message,
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
  final public function assertLessThanOrEqual(
    mixed $expected,
    mixed $actual,
    string $message = '',
  ): bool {

    return
      AssertLessThanOrEqual::evaluate($this, $expected, $actual, $message);

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
  final public function assertAttributeLessThanOrEqual(
    mixed $expected,
    string $actualAttributeName,
    mixed $actualClassOrObject,
    string $message = '',
  ): bool {

    return AssertAttributeLessThanOrEqual::evaluate(
      $this,
      $expected,
      $actualAttributeName,
      $actualClassOrObject,
      $message,
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
  final public function assertFileEquals(
    string $expected,
    string $actual,
    string $message = '',
    bool $canonicalize = false,
    bool $ignoreCase = false,
  ): bool {

    return AssertFileEquals::evaluate(
      $this,
      $expected,
      $actual,
      $message,
      $canonicalize,
      $ignoreCase,
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
  final public function assertFileNotEquals(
    string $expected,
    string $actual,
    string $message = '',
    bool $canonicalize = false,
    bool $ignoreCase = false,
  ): bool {

    return AssertFileNotEquals::evaluate(
      $this,
      $expected,
      $actual,
      $message,
      $canonicalize,
      $ignoreCase,
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
  final public function assertStringEqualsFile(
    string $expectedFile,
    string $actualString,
    string $message = '',
    bool $canonicalize = false,
    bool $ignoreCase = false,
  ): bool {

    return AssertStringEqualsFile::evaluate(
      $this,
      $expectedFile,
      $actualString,
      $message,
      $canonicalize,
      $ignoreCase,
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
  final public function assertStringNotEqualsFile(
    string $expectedFile,
    string $actualString,
    string $message = '',
    bool $canonicalize = false,
    bool $ignoreCase = false,
  ): bool {

    return AssertStringNotEqualsFile::evaluate(
      $this,
      $expectedFile,
      $actualString,
      $message,
      $canonicalize,
      $ignoreCase,
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
  final public function assertFileExists(
    string $filename,
    string $message = '',
  ): bool {

    return AssertFileExists::evaluate($this, $filename, $message);

  }

  /**
   * Asserts that a file does not exist.
   *
   * @param string $filename
   * @param string $message
   *
   * @since Method available since Release 3.0.0
   */
  final public function assertFileNotExists(
    string $filename,
    string $message = '',
  ): bool {

    return AssertFileNotExists::evaluate($this, $filename, $message);

  }

  /**
   * Asserts that a condition is true.
   *
   * @param bool   $condition
   * @param string $message
   *
   * @throws PHPUnit_Framework_AssertionFailedError
   */
  final public function assertTrue(
    bool $condition,
    string $message = '',
  ): bool {

    return AssertTrue::evaluate($this, $condition, $message);

  }

  /**
   * Asserts that a condition is not true.
   *
   * @param bool   $condition
   * @param string $message
   *
   * @throws PHPUnit_Framework_AssertionFailedError
   */
  final public function assertNotTrue(
    bool $condition,
    string $message = '',
  ): bool {

    return AssertNotTrue::evaluate($this, $condition, $message);

  }

  /**
   * Asserts that a condition is false.
   *
   * @param bool   $condition
   * @param string $message
   *
   * @throws PHPUnit_Framework_AssertionFailedError
   */
  final public function assertFalse(
    bool $condition,
    string $message = '',
  ): bool {
    return $this->assertNotTrue($condition, $message);
  }

  /**
   * Asserts that a condition is not false.
   *
   * @param bool   $condition
   * @param string $message
   *
   * @throws PHPUnit_Framework_AssertionFailedError
   */
  final public function assertNotFalse(
    bool $condition,
    string $message = '',
  ): bool {
    return $this->assertTrue($condition, $message);
  }

  /**
   * Asserts that a variable is null.
   *
   * @param mixed  $actual
   * @param string $message
   */
  final public function assertNull(mixed $actual, string $message = ''): bool {

    return AssertNull::evaluate($this, $actual, $message);

  }

  /**
   * Asserts that a variable is not null.
   *
   * @param mixed  $actual
   * @param string $message
   */
  final public function assertNotNull(
    mixed $actual,
    string $message = '',
  ): bool {

    return AssertNotNull::evaluate($this, $actual, $message);

  }

  /**
   * Asserts that a variable is finite.
   *
   * @param mixed  $actual
   * @param string $message
   */
  final public function assertFinite(
    mixed $actual,
    string $message = '',
  ): bool {

    return AssertFinite::evaluate($this, $actual, $message);

  }

  /**
   * Asserts that a variable is infinite.
   *
   * @param mixed  $actual
   * @param string $message
   */
  final public function assertInfinite(
    mixed $actual,
    string $message = '',
  ): bool {

    return AssertInfinite::evaluate($this, $actual, $message);

  }

  /**
   * Asserts that a variable is nan.
   *
   * @param mixed  $actual
   * @param string $message
   */
  final public function assertNan(mixed $actual, string $message = ''): bool {

    return AssertNan::evaluate($this, $actual, $message);

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
  final public function assertStringMatchesFormat(
    string $format,
    string $string,
    string $message = '',
  ): bool {

    return
      AssertStringMatchesFormat::evaluate($this, $format, $string, $message);

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
  final public function assertObjectNotHasAttribute(
    string $attributeName,
    mixed $object,
    string $message = '',
  ): bool {

    return AssertObjectNotHasAttribute::evaluate(
      $this,
      $attributeName,
      $object,
      $message,
    );

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
  final public function assertObjectHasAttribute(
    string $attributeName,
    mixed $object,
    string $message = '',
  ): bool {

    return AssertObjectHasAttribute::evaluate(
      $this,
      $attributeName,
      $object,
      $message,
    );

  }

  /**
   * Asserts that a class does not have a specified static attribute.
   *
   * @param string $attributeName
   * @param string $className
   * @param string $message
   *
   * @since Method available since Release 3.1.0
   */
  final public function assertClassNotHasStaticAttribute(
    string $attributeName,
    mixed $className,
    string $message = '',
  ): bool {

    return AssertClassNotHasStaticAttribute::evaluate(
      $this,
      $attributeName,
      $className,
      $message,
    );

  }

  /**
   * Asserts that a class has a specified static attribute.
   *
   * @param string $attributeName
   * @param string $className
   * @param string $message
   *
   * @since Method available since Release 3.1.0
   */
  final public function assertClassHasStaticAttribute(
    string $attributeName,
    string $className,
    string $message = '',
  ): bool {

    return AssertClassHasStaticAttribute::evaluate(
      $this,
      $attributeName,
      $className,
      $message,
    );

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
  final public function assertClassNotHasAttribute(
    string $attributeName,
    string $className,
    string $message = '',
  ): bool {

    return AssertClassNotHasAttribute::evaluate(
      $this,
      $attributeName,
      $className,
      $message,
    );

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
  final public function assertClassHasAttribute(
    string $attributeName,
    string $className,
    string $message = '',
  ): bool {

    return AssertClassHasAttribute::evaluate(
      $this,
      $attributeName,
      $className,
      $message,
    );

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
  final public function assertSame(
    mixed $expected,
    mixed $actual,
    string $message = '',
  ): bool {

    return AssertSame::evaluate($this, $expected, $actual, $message);

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
  final public function assertAttributeSame(
    mixed $expected,
    string $actualAttributeName,
    mixed $actualClassOrObject,
    string $message = '',
  ): bool {

    return AssertAttributeSame::evaluate(
      $this,
      $expected,
      $actualAttributeName,
      $actualClassOrObject,
      $message,
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
  final public function assertNotSame(
    mixed $expected,
    mixed $actual,
    string $message = '',
  ): bool {

    return AssertNotSame::evaluate($this, $expected, $actual, $message);

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
  final public function assertAttributeNotSame(
    mixed $expected,
    string $actualAttributeName,
    mixed $actualClassOrObject,
    string $message = '',
  ): bool {

    return AssertAttributeNotSame::evaluate(
      $this,
      $expected,
      $actualAttributeName,
      $actualClassOrObject,
      $message,
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
  final public function assertInstanceOf(
    string $expected,
    mixed $actual,
    string $message = '',
  ): bool {

    return AssertInstanceOf::evaluate($this, $expected, $actual, $message);

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
  final public function assertAttributeInstanceOf(
    string $expected,
    string $attributeName,
    mixed $classOrObject,
    string $message = '',
  ): bool {

    return AssertAttributeInstanceOf::evaluate(
      $this,
      $expected,
      $attributeName,
      $classOrObject,
      $message,
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
  final public function assertNotInstanceOf(
    string $expected,
    mixed $actual,
    string $message = '',
  ): bool {

    return AssertNotInstanceOf::evaluate($this, $expected, $actual, $message);

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
  final public function assertAttributeNotInstanceOf(
    string $expected,
    string $attributeName,
    mixed $classOrObject,
    string $message = '',
  ): bool {

    return AssertAttributeNotInstanceOf::evaluate(
      $this,
      $expected,
      $attributeName,
      $classOrObject,
      $message,
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
  final public function assertInternalType(
    string $expected,
    mixed $actual,
    string $message = '',
  ): bool {

    return AssertInternalType::evaluate($this, $expected, $actual, $message);

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
  final public function assertAttributeInternalType(
    string $expected,
    string $attributeName,
    mixed $classOrObject,
    string $message = '',
  ): bool {

    return AssertAttributeInternalType::evaluate(
      $this,
      $expected,
      $attributeName,
      $classOrObject,
      $message,
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
  final public function assertNotInternalType(
    string $expected,
    mixed $actual,
    string $message = '',
  ): bool {

    return
      AssertNotInternalType::evaluate($this, $expected, $actual, $message);

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
  final public function assertAttributeNotInternalType(
    string $expected,
    string $attributeName,
    mixed $classOrObject,
    string $message = '',
  ): bool {

    return AssertAttributeNotInternalType::evaluate(
      $this,
      $expected,
      $attributeName,
      $classOrObject,
      $message,
    );

  }

  /**
   * Asserts that a string matches a given regular expression.
   *
   * @param string $pattern
   * @param string $string
   * @param string $message
   */
  final public function assertRegExp(
    string $pattern,
    string $string,
    string $message = '',
  ): bool {

    return AssertRegExp::evaluate($this, $pattern, $string, $message);

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
  final public function assertNotRegExp(
    string $pattern,
    string $string,
    string $message = '',
  ): bool {

    return AssertNotRegExp::evaluate($this, $pattern, $string, $message);

  }

  /**
   * Assert that the size of two arrays (or `Countable` or `Traversable` objects)
   * is the same.
   *
   * @param array|Countable|Traversable $expected
   * @param array|Countable|Traversable $actual
   * @param string                      $message
   */
  final public function assertSameSize(
    mixed $expected,
    mixed $actual,
    string $message = '',
  ): bool {

    return AssertSameSize::evaluate($this, $expected, $actual, $message);

  }

  /**
   * Assert that the size of two arrays (or `Countable` or `Traversable` objects)
   * is not the same.
   *
   * @param array|Countable|Traversable $expected
   * @param array|Countable|Traversable $actual
   * @param string                      $message
   */
  final public function assertNotSameSize(
    mixed $expected,
    mixed $actual,
    string $message = '',
  ): bool {

    return AssertNotSameSize::evaluate($this, $expected, $actual, $message);

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
  final public function assertStringNotMatchesFormat(
    string $format,
    string $string,
    string $message = '',
  ): bool {

    return AssertStringNotMatchesFormat::evaluate(
      $this,
      $format,
      $string,
      $message,
    );

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
  final public function assertStringMatchesFormatFile(
    string $formatFile,
    string $string,
    string $message = '',
  ): bool {

    return AssertStringMatchesFormatFile::evaluate(
      $this,
      $formatFile,
      $string,
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
   * Fails a test with the given message.
   *
   * @param string $message
   *
   * @throws PHPUnit_Framework_AssertionFailedError
   */
  final public function fail(string $message = ''): void {
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

  /**
   * Return the current assertion count.
   *
   * @return int
   *
   * @since Method available since Release 3.3.3
   */
  final public function getCount(): int {
    return $this->counter()->get();
  }

  /**
   * Reset the assertion counter.
   *
   * @since Method available since Release 3.3.3
   */
  final public function resetCount(): bool {
    return $this->counter()->reset();
  }

}
