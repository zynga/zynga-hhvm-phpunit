<?hh // strict

namespace Zynga\PHPUnit\V2\Constraints;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use Zynga\PHPUnit\V2\Constraints\Base;
use Zynga\Framework\ReflectionCache\V1\ReflectionClasses;

/**
 * Constraint that asserts that the class it is evaluated for has a given
 * static attribute.
 *
 * The attribute name is passed in the constructor.
 *
 * @since Class available since Release 3.1.0
 */
class ClassHasStaticAttributeConstraint extends Base {
  private string $attributeName = '';

  public function setExpected(mixed $value): bool {
    $this->attributeName = strval($value);
    return true;
  }

  public function resetExpected(): bool {
    $this->attributeName = '';
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

    $class = ReflectionClasses::getReflection($other);

    if ($class == null) {
      return false;
    }

    if ($class->hasProperty($this->attributeName)) {

      $attribute = $class->getProperty($this->attributeName);

      return $attribute->isStatic();

    }

    return false;

  }

  /**
   * Returns a string representation of the constraint.
   *
   * @return string
   *
   * @since Method available since Release 3.3.0
   */
  public function toString(): string {
    return sprintf('has static attribute "%s"', $this->attributeName);
  }

}
