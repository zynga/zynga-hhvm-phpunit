<?hh // strict

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace Zynga\PHPUnit\V2;

// --
// JEO: So we broke up the assertions into individual static classes to make
// them more readable and to keep the 'outward' api interface as light as possible.
//
// As the autoloader is lazy, it lowers the total amount of code loaded in most
// use cases.
// --
use Zynga\PHPUnit\V2\Assertions\Count as AssertionsCount;
use Zynga\PHPUnit\V2\Assertions\AssertArrayHasKey;
use Zynga\PHPUnit\V2\Assertions\AssertArrayNotHasKey;
use Zynga\PHPUnit\V2\Assertions\AssertArraySubset;
use Zynga\PHPUnit\V2\Assertions\AssertAttributeContains;
use Zynga\PHPUnit\V2\Assertions\AssertAttributeContainsOnly;
use Zynga\PHPUnit\V2\Assertions\AssertAttributeCount;
use Zynga\PHPUnit\V2\Assertions\AssertAttributeEmpty;
use Zynga\PHPUnit\V2\Assertions\AssertAttributeEquals;
use Zynga\PHPUnit\V2\Assertions\AssertAttributeGreaterThan;
use Zynga\PHPUnit\V2\Assertions\AssertAttributeGreaterThanOrEqual;
use Zynga\PHPUnit\V2\Assertions\AssertAttributeInstanceOf;
use Zynga\PHPUnit\V2\Assertions\AssertAttributeInternalType;
use Zynga\PHPUnit\V2\Assertions\AssertAttributeLessThan;
use Zynga\PHPUnit\V2\Assertions\AssertAttributeLessThanOrEqual;
use Zynga\PHPUnit\V2\Assertions\AssertAttributeNotContains;
use Zynga\PHPUnit\V2\Assertions\AssertAttributeNotContainsOnly;
use Zynga\PHPUnit\V2\Assertions\AssertAttributeNotCount;
use Zynga\PHPUnit\V2\Assertions\AssertAttributeNotEmpty;
use Zynga\PHPUnit\V2\Assertions\AssertAttributeNotEquals;
use Zynga\PHPUnit\V2\Assertions\AssertAttributeNotInstanceOf;
use Zynga\PHPUnit\V2\Assertions\AssertAttributeNotInternalType;
use Zynga\PHPUnit\V2\Assertions\AssertAttributeNotSame;
use Zynga\PHPUnit\V2\Assertions\AssertAttributeSame;
use Zynga\PHPUnit\V2\Assertions\AssertClassHasAttribute;
use Zynga\PHPUnit\V2\Assertions\AssertClassHasStaticAttribute;
use Zynga\PHPUnit\V2\Assertions\AssertClassNotHasAttribute;
use Zynga\PHPUnit\V2\Assertions\AssertClassNotHasStaticAttribute;
use Zynga\PHPUnit\V2\Assertions\AssertClearStatCache;
use Zynga\PHPUnit\V2\Assertions\AssertContains;
use Zynga\PHPUnit\V2\Assertions\AssertContainsOnly;
use Zynga\PHPUnit\V2\Assertions\AssertContainsOnlyInstancesOf;
use Zynga\PHPUnit\V2\Assertions\AssertCount;
use Zynga\PHPUnit\V2\Assertions\AssertEmpty;
use Zynga\PHPUnit\V2\Assertions\AssertEquals;
use Zynga\PHPUnit\V2\Assertions\AssertEqualXMLStructure;
use Zynga\PHPUnit\V2\Assertions\AssertExceptionRegexpMessage;
use Zynga\PHPUnit\V2\Assertions\AssertExceptionSameClass;
use Zynga\PHPUnit\V2\Assertions\AssertExceptionSameMessage;
use Zynga\PHPUnit\V2\Assertions\AssertFileEquals;
use Zynga\PHPUnit\V2\Assertions\AssertFileExists;
use Zynga\PHPUnit\V2\Assertions\AssertFileNotEquals;
use Zynga\PHPUnit\V2\Assertions\AssertFileNotExists;
use Zynga\PHPUnit\V2\Assertions\AssertFinite;
use Zynga\PHPUnit\V2\Assertions\AssertGreaterThan;
use Zynga\PHPUnit\V2\Assertions\AssertGreaterThanOrEqual;
use Zynga\PHPUnit\V2\Assertions\AssertInfinite;
use Zynga\PHPUnit\V2\Assertions\AssertInstanceOf;
use Zynga\PHPUnit\V2\Assertions\AssertInternalType;
use Zynga\PHPUnit\V2\Assertions\AssertJson;
use Zynga\PHPUnit\V2\Assertions\AssertJsonFileEqualsJsonFile;
use Zynga\PHPUnit\V2\Assertions\AssertJsonFileNotEqualsJsonFile;
use Zynga\PHPUnit\V2\Assertions\AssertJsonStringEqualsJsonString;
use Zynga\PHPUnit\V2\Assertions\AssertJsonStringEqualsJsonFile;
use Zynga\PHPUnit\V2\Assertions\AssertJsonStringNotEqualsJsonFile;
use Zynga\PHPUnit\V2\Assertions\AssertJsonStringNotEqualsJsonString;
use Zynga\PHPUnit\V2\Assertions\AssertLessThan;
use Zynga\PHPUnit\V2\Assertions\AssertLessThanOrEqual;
use Zynga\PHPUnit\V2\Assertions\AssertNan;
use Zynga\PHPUnit\V2\Assertions\AssertNotContainsOnly;
use Zynga\PHPUnit\V2\Assertions\AssertNotContains;
use Zynga\PHPUnit\V2\Assertions\AssertNotCount;
use Zynga\PHPUnit\V2\Assertions\AssertNotEmpty;
use Zynga\PHPUnit\V2\Assertions\AssertNotEquals;
use Zynga\PHPUnit\V2\Assertions\AssertNotInstanceOf;
use Zynga\PHPUnit\V2\Assertions\AssertNotInternalType;
use Zynga\PHPUnit\V2\Assertions\AssertNotNull;
use Zynga\PHPUnit\V2\Assertions\AssertNotRegExp;
use Zynga\PHPUnit\V2\Assertions\AssertNotSame;
use Zynga\PHPUnit\V2\Assertions\AssertNotSameSize;
use Zynga\PHPUnit\V2\Assertions\AssertNotTrue;
use Zynga\PHPUnit\V2\Assertions\AssertNull;
use Zynga\PHPUnit\V2\Assertions\AssertObjectHasAttribute;
use Zynga\PHPUnit\V2\Assertions\AssertObjectNotHasAttribute;
use Zynga\PHPUnit\V2\Assertions\AssertRegExp;
use Zynga\PHPUnit\V2\Assertions\AssertSame;
use Zynga\PHPUnit\V2\Assertions\AssertSameSize;
use Zynga\PHPUnit\V2\Assertions\AssertStringEndsWith;
use Zynga\PHPUnit\V2\Assertions\AssertStringEndsNotWith;
use Zynga\PHPUnit\V2\Assertions\AssertStringEqualsFile;
use Zynga\PHPUnit\V2\Assertions\AssertStringMatchesFormat;
use Zynga\PHPUnit\V2\Assertions\AssertStringMatchesFormatFile;
use Zynga\PHPUnit\V2\Assertions\AssertStringNotEqualsFile;
use Zynga\PHPUnit\V2\Assertions\AssertStringNotMatchesFormat;
use Zynga\PHPUnit\V2\Assertions\AssertStringNotMatchesFormatFile;
use Zynga\PHPUnit\V2\Assertions\AssertStringStartsWith;
use Zynga\PHPUnit\V2\Assertions\AssertStringStartsNotWith;
use Zynga\PHPUnit\V2\Assertions\AssertThat;
use Zynga\PHPUnit\V2\Assertions\AssertTrue;
use Zynga\PHPUnit\V2\Assertions\AssertXmlFileEqualsXmlFile;
use Zynga\PHPUnit\V2\Assertions\AssertXmlFileNotEqualsXmlFile;
use Zynga\PHPUnit\V2\Assertions\AssertXmlStringEqualsXmlFile;
use Zynga\PHPUnit\V2\Assertions\AssertXmlStringEqualsXmlString;
use Zynga\PHPUnit\V2\Assertions\AssertXmlStringNotEqualsXmlFile;
use Zynga\PHPUnit\V2\Assertions\AssertXmlStringNotEqualsXmlString;
use Zynga\PHPUnit\V2\Assertions\GetObjectAttribute;
use Zynga\PHPUnit\V2\Assertions\GetStaticAttribute;
use Zynga\PHPUnit\V2\Assertions\ReadAttribute;
use Zynga\PHPUnit\V2\Interfaces\ConstraintInterface;
use SebastianBergmann\PHPUnit\Exceptions\AssertionFailedException;
use SebastianBergmann\PHPUnit\Exceptions\TestError\IncompleteException;
use SebastianBergmann\PHPUnit\Exceptions\TestError\SkippedException;
use Zynga\PHPUnit\V2\Test\Base;

use \DOMElement;

abstract class Assertions extends Base {

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
   * @throws AssertionFailedException
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
   * @throws AssertionFailedException
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
   * @throws AssertionFailedException
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
   * @throws AssertionFailedException
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
   * @throws AssertionFailedException
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
   * @throws AssertionFailedException
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
   * Asserts that a string does not match a given format string.
   *
   * @param string $formatFile
   * @param string $string
   * @param string $message
   *
   * @since Method available since Release 3.5.0
   */
  final public function assertStringNotMatchesFormatFile(
    string $formatFile,
    string $string,
    string $message = '',
  ): bool {

    return AssertStringNotMatchesFormatFile::evaluate(
      $this,
      $formatFile,
      $string,
      $message,
    );

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
  final public function assertStringStartsWith(
    string $prefix,
    string $string,
    string $message = '',
  ): bool {

    return
      AssertStringStartsWith::evaluate($this, $prefix, $string, $message);

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
  final public function assertStringStartsNotWith(
    string $prefix,
    string $string,
    string $message = '',
  ): bool {

    return
      AssertStringStartsNotWith::evaluate($this, $prefix, $string, $message);

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
  final public function assertStringEndsWith(
    string $suffix,
    string $string,
    string $message = '',
  ): bool {

    return AssertStringEndsWith::evaluate($this, $suffix, $string, $message);

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
  final public function assertStringEndsNotWith(
    string $suffix,
    string $string,
    string $message = '',
  ): bool {

    return
      AssertStringEndsNotWith::evaluate($this, $suffix, $string, $message);

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
  final public function assertXmlFileEqualsXmlFile(
    string $expectedFile,
    string $actualFile,
    string $message = '',
  ): bool {

    return AssertXmlFileEqualsXmlFile::evaluate(
      $this,
      $expectedFile,
      $actualFile,
      $message,
    );

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
  final public function assertXmlFileNotEqualsXmlFile(
    string $expectedFile,
    string $actualFile,
    string $message = '',
  ): bool {

    return AssertXmlFileNotEqualsXmlFile::evaluate(
      $this,
      $expectedFile,
      $actualFile,
      $message,
    );

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
  final public function assertXmlStringEqualsXmlFile(
    string $expectedFile,
    string $actualXml,
    string $message = '',
  ): bool {

    return AssertXmlStringEqualsXmlFile::evaluate(
      $this,
      $expectedFile,
      $actualXml,
      $message,
    );

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
  final public function assertXmlStringNotEqualsXmlFile(
    string $expectedFile,
    string $actualXml,
    string $message = '',
  ): bool {

    return AssertXmlStringNotEqualsXmlFile::evaluate(
      $this,
      $expectedFile,
      $actualXml,
      $message,
    );

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
  final public function assertXmlStringEqualsXmlString(
    string $expectedXml,
    string $actualXml,
    string $message = '',
  ): bool {

    return AssertXmlStringEqualsXmlString::evaluate(
      $this,
      $expectedXml,
      $actualXml,
      $message,
    );

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
  public function assertEqualXMLStructure(
    DOMElement $expectedElement,
    DOMElement $actualElement,
    bool $checkAttributes = false,
    string $message = '',
  ): bool {

    return AssertEqualXMLStructure::evaluate(
      $this,
      $expectedElement,
      $actualElement,
      $checkAttributes,
      $message,
    );

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
  final public function assertXmlStringNotEqualsXmlString(
    string $expectedXml,
    string $actualXml,
    string $message = '',
  ): bool {

    return AssertXmlStringNotEqualsXmlString::evaluate(
      $this,
      $expectedXml,
      $actualXml,
      $message,
    );

  }

  /**
   * Asserts that a string is a valid JSON string.
   *
   * @param string $actualJson
   * @param string $message
   *
   * @since Method available since Release 3.7.20
   */
  final public function assertJson(
    string $actualJson,
    string $message = '',
  ): bool {

    return AssertJson::evaluate($this, $actualJson, $message);

  }

  /**
   * Asserts that two given JSON encoded objects or arrays are equal.
   *
   * @param string $expectedJson
   * @param string $actualJson
   * @param string $message
   */
  final public function assertJsonStringEqualsJsonString(
    string $expectedJson,
    string $actualJson,
    string $message = '',
  ): bool {

    return AssertJsonStringEqualsJsonString::evaluate(
      $this,
      $expectedJson,
      $actualJson,
      $message,
    );

  }

  /**
   * Asserts that two given JSON encoded objects or arrays are not equal.
   *
   * @param string $expectedJson
   * @param string $actualJson
   * @param string $message
   */
  final public function assertJsonStringNotEqualsJsonString(
    string $expectedJson,
    string $actualJson,
    string $message = '',
  ): bool {

    return AssertJsonStringNotEqualsJsonString::evaluate(
      $this,
      $expectedJson,
      $actualJson,
      $message,
    );

  }

  /**
   * Asserts that the generated JSON encoded object and the content of the given file are equal.
   *
   * @param string $expectedFile
   * @param string $actualJson
   * @param string $message
   */
  final public function assertJsonStringEqualsJsonFile(
    string $expectedFile,
    string $actualJson,
    string $message = '',
  ): bool {

    return AssertJsonStringEqualsJsonFile::evaluate(
      $this,
      $expectedFile,
      $actualJson,
      $message,
    );

  }

  /**
   * Asserts that the generated JSON encoded object and the content of the given file are not equal.
   *
   * @param string $expectedFile
   * @param string $actualJson
   * @param string $message
   */
  final public function assertJsonStringNotEqualsJsonFile(
    string $expectedFile,
    string $actualJson,
    string $message = '',
  ): bool {

    return AssertJsonStringNotEqualsJsonFile::evaluate(
      $this,
      $expectedFile,
      $actualJson,
      $message,
    );

  }

  /**
   * Asserts that two JSON files are equal.
   *
   * @param string $expectedFile
   * @param string $actualFile
   * @param string $message
   */
  final public function assertJsonFileEqualsJsonFile(
    string $expectedFile,
    string $actualFile,
    string $message = '',
  ): bool {

    return AssertJsonFileEqualsJsonFile::evaluate(
      $this,
      $expectedFile,
      $actualFile,
      $message,
    );

  }

  /**
   * Asserts that two JSON files are not equal.
   *
   * @param string $expectedFile
   * @param string $actualFile
   * @param string $message
   */
  final public function assertJsonFileNotEqualsJsonFile(
    string $expectedFile,
    string $actualFile,
    string $message = '',
  ): bool {

    return AssertJsonFileNotEqualsJsonFile::evaluate(
      $this,
      $expectedFile,
      $actualFile,
      $message,
    );

  }

  /**
   * Determines if a given exception, matches another exception's class.
   *
   * @param mixed                        $expected
   * @param mixed                        $actual
   * @param string                       $message
   *
   * @since Method available since Release 2.1.5-darkseid
   */
  final public function assertExceptionSameClass(
    mixed $expected,
    mixed $actual,
    string $message = '',
  ): bool {

    return
      AssertExceptionSameClass::evaluate($this, $expected, $actual, $message);

  }

  /**
   * Determines if a given exception, matches another exception's message.
   *
   * @param mixed                        $expected
   * @param mixed                        $actual
   * @param string                       $message
   *
   * @since Method available since Release 2.1.5-darkseid
   */
  final public function assertExceptionSameMessage(
    mixed $expected,
    mixed $actual,
    string $message = '',
  ): bool {

    return AssertExceptionSameMessage::evaluate(
      $this,
      $expected,
      $actual,
      $message,
    );

  }

  /**
   * Determines if a given exception, matches another exception's message.
   *
   * @param mixed                        $expected
   * @param mixed                        $actual
   * @param string                       $message
   *
   * @since Method available since Release 2.1.5-darkseid
   */
  final public function assertExceptionRegexpMessage(
    string $expected,
    mixed $actual,
    string $message = '',
  ): bool {

    return AssertExceptionRegexpMessage::evaluate(
      $this,
      $expected,
      $actual,
      $message,
    );

  }

  final public function assertClearStatCache(string $message = ''): bool {

    return AssertClearStatCache::evaluate($this, $message);

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
   * @throws AssertionFailedException
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

  /**
   * Mark the test as incomplete.
   *
   * @param string $message
   *
   * @throws IncompleteException
   *
   * @since Method available since Release 3.0.0
   */
  final public function markTestIncomplete(string $message = ''): void {
    throw new IncompleteException($message);
  }

  /**
   * Mark the test as skipped.
   *
   * @param string $message
   *
   * @throws SkippedException
   *
   * @since Method available since Release 3.0.0
   */
  final public function markTestSkipped(string $message = ''): void {
    throw new SkippedException($message);
  }

}
