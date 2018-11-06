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


use SebastianBergmann\PHPUnit\Constraint\ArrayHasKey;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentExceptionFactory;
use SebastianBergmann\PHPUnit\Interfaces\ConstraintInterface;
use Zynga\Framework\ReflectionCache\V1\ReflectionClasses;

use \ArrayAccess;

class Assertions {

  /**
   * @var int
   */
  private static int $count = 0;

  /**
   * Returns a PHPUnit_Framework_Constraint_ArrayHasKey matcher object.
   *
   * @param mixed $key
   *
   * @return PHPUnit_Framework_Constraint_ArrayHasKey
   *
   * @since Method available since Release 3.0.0
   */
  public function arrayHasKey(mixed $key): ConstraintInterface {
    return new ArrayHasKey($key);
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

    $constraint = $this->arrayHasKey($key);

    return $this->assertThat($array, $constraint, $message);

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

    self::$count += count($constraint);

    return $constraint->evaluate($value, $message);

  }

}
