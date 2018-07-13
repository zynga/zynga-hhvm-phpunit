<?hh // strict

namespace PHPUnit\Constraint;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use PHPUnit\Constraint\ClassHasAttributeConstraint;
use \ReflectionClass;

/**
 * Constraint that asserts that the class it is evaluated for has a given
 * static attribute.
 *
 * The attribute name is passed in the constructor.
 *
 * @since Class available since Release 3.1.0
 */
class ClassHasStaticAttributeConstraint extends ClassHasAttributeConstraint {
    /**
     * Evaluates the constraint for parameter $other. Returns true if the
     * constraint is met, false otherwise.
     *
     * @param mixed $other Value or object to evaluate.
     *
     * @return bool
     */
    public function matches(mixed $other): bool {

      $class = new ReflectionClass($other);

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
      return sprintf(
        'has static attribute "%s"',
        $this->attributeName
      );
    }
}
