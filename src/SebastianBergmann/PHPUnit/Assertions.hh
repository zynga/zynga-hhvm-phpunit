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

use SebastianBergmann\PHPUnit\Assertions\Count as AssertionsCount;
use SebastianBergmann\PHPUnit\Constraints\Factory as ConstraintFactory;
use SebastianBergmann\PHPUnit\Constraints\ArrayHasKeyConstraint;
use SebastianBergmann\PHPUnit\Constraints\ArraySubsetConstraint;
use SebastianBergmann\PHPUnit\Constraints\StringContainsConstraint;
use SebastianBergmann\PHPUnit\Constraints\TraversableContainsConstraint;
use SebastianBergmann\PHPUnit\Constraints\NotConstraint;
use SebastianBergmann\PHPUnit\Exceptions\AssertionFailedException;
use SebastianBergmann\PHPUnit\Exceptions\ExpectationFailedException;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentExceptionFactory;
use SebastianBergmann\PHPUnit\Interfaces\ConstraintInterface;
use Zynga\Framework\ReflectionCache\V1\ReflectionClasses;

use \ArrayAccess;

class Assertions {

  private ?AssertionsCount $count;

  final public function count(): AssertionsCount {
    if (!$this->count instanceof AssertionsCount) {
      $this->count = new AssertionsCount();
    }
    return $this->count;
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

    if (!(is_int($key) || is_string($key))) {
      throw InvalidArgumentExceptionFactory::factory(1, 'integer or string');
    }

    if (!(is_array($array) || $array instanceof ArrayAccess)) {
      throw InvalidArgumentExceptionFactory::factory(
        2,
        'array or ArrayAccess',
      );
    }

    $constraint = ConstraintFactory::factory('ArrayHasKey');

    $constraint->setExpected($key);

    return $this->assertThat($array, $constraint, $message);

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

    if (!is_array($subset)) {
      throw InvalidArgumentExceptionFactory::factory(
        1,
        'array or ArrayAccess',
      );
    }

    if (!is_array($array)) {
      throw InvalidArgumentExceptionFactory::factory(
        2,
        'array or ArrayAccess',
      );
    }

    $constraint = ConstraintFactory::factory('ArraySubset');

    $constraint->setExpected($subset);

    if ($constraint instanceof ArraySubsetConstraint) {
      $constraint->setStrict($strict);
    }

    return $this->assertThat($array, $constraint, $message);

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

    if (!(is_int($key) || is_string($key))) {
      throw InvalidArgumentExceptionFactory::factory(1, 'integer or string');
    }

    if (!(is_array($array) || $array instanceof ArrayAccess)) {
      throw InvalidArgumentExceptionFactory::factory(
        2,
        'array or ArrayAccess',
      );
    }

    $hasKeyConstraint = ConstraintFactory::factory('ArrayHasKey');
    $hasKeyConstraint->setExpected($key);

    $notConstraint = ConstraintFactory::factory('Not');
    $notConstraint->setExpected($hasKeyConstraint);

    return $this->assertThat($array, $notConstraint, $message);

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
        is_object($haystack) && $haystack instanceof Traversable) {

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
      throw new AssertionFailedException($e->getMessage(), $e->getCode());
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

}
