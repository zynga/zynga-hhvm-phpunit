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

use PHPUnit\Constraint\Base as ConstraintBase;
use PHPUnit\Util\InvalidArgumentHelperUtil;
use \ReflectionClass;
use \ReflectionException;

/**
 * Constraint that asserts that the object it is evaluated for is an instance
 * of a given class.
 *
 * The expected class name is passed in the constructor.
 *
 * @since Class available since Release 3.0.0
 */
class IsInstanceOfConstraint extends ConstraintBase {

    /**
     * @var string
     */
    protected string $className;

    /**
     * @param string $className
     */
    public function __construct(mixed $className) {

      parent::__construct();

      $className = strval($className);

      if ( class_exists($className) || interface_exists($className) ) {
        throw InvalidArgumentHelperUtil::factory(1, 'class or interface name');
      }

      $this->className = strval($className);

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

      $otherReflected = new ReflectionClass($other);

      if ( $otherReflected->isSubclassOf($this->className) ) {
        return true;
      }

      if ( $otherReflected->implementsInterface($this->className) ) {
        return true;
      }

      return false;

    }

    /**
     * Returns the description of the failure
     *
     * The beginning of failure messages is "Failed asserting that" in most
     * cases. This method should return the second part of that sentence.
     *
     * @param mixed $other Evaluated value or object.
     *
     * @return string
     */
    public function failureDescription(mixed $other): string {

      return sprintf(
        '%s is an instance of %s "%s"',
        $this->exporter->shortenedExport($other),
        $this->getType(),
        $this->className
      );

    }

    /**
     * Returns a string representation of the constraint.
     *
     * @return string
     */
    public function toString(): string {

      return sprintf(
        'is instance of %s "%s"',
        $this->getType(),
        $this->className
      );

    }

    private function getType(): string {

      try {

        $reflection = new ReflectionClass($this->className);

        if ($reflection->isInterface()) {
          return 'interface';
        }

      } catch (ReflectionException $e) {
      }

      return 'class';

    }

}
