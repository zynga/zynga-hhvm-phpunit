<?hh // strict

namespace SebastianBergmann\PHPUnit\Constraints;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use SebastianBergmann\PHPUnit\Constraints\Base;
use SebastianBergmann\PHPUnit\Environment\LanguageTypes;

/**
 * Constraint that asserts that the value it is evaluated for is of a
 * specified type.
 *
 * The expected value is passed in the constructor.
 *
 * @since Class available since Release 3.0.0
 */
class IsNativeTypeConstraint extends Base {

  private string $type = '';

  /**
   * @param string $type
   *
   * @throws PHPUnit_Framework_Exception
   */
  public function setExpected(mixed $type): bool {

    if (!is_string($type)) {
      return false;
    }

    if (LanguageTypes::isNativeType($type) === true) {
      $this->type = $type;
      return true;
    }

    return false;

  }

  public function resetExpected(): bool {
    $this->type = '';
    return true;
  }

  /**
   * Evaluates the constraint for parameter $other. Returns true if the
   * constraint is met, false otherwise.
   *
   * @param mixed $other Value or object to evaluate.
   *
   * @return bool
   */
  public function matches(mixed $other): bool {

    switch ($this->type) {

      case 'numeric':
        return is_numeric($other);

      case 'integer':
      case 'int':
        return is_int($other);

      case 'double':
      case 'float':
      case 'real':
        return is_float($other);

      case 'string':
        return is_string($other);

      case 'boolean':
      case 'bool':
        return is_bool($other);

      case 'null':
        return is_null($other);

      case 'array':
        return is_array($other);

      case 'object':
        return is_object($other);

      case 'resource':
        return is_resource($other) || is_string(@get_resource_type($other));

      case 'scalar':
        return is_scalar($other);

      case 'callable':
        return is_callable($other);
    }

    return false;

  }

  /**
   * Returns a string representation of the constraint.
   *
   * @return string
   */
  public function toString(): string {
    return sprintf('is of type "%s"', $this->type);
  }

}
