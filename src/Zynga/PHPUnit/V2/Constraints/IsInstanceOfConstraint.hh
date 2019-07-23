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
class IsInstanceOfConstraint extends Base {
  private string $className = '';

  public function setExpected(mixed $className): bool {
    if (is_string($className) &&
        (class_exists($className) || interface_exists($className))) {
      $this->className = $className;
      return true;
    }
    var_dump($className);
    return false;
  }

  public function resetExpected(): bool {
    $this->className = '';
    return true;
  }

  private function _getParentClasses(
    ReflectionClass $parentalUnit,
  ): Vector<string> {

    $parents = Vector {};

    // add yourself.
    $parents->add($parentalUnit->getName());

    $grandParent = $parentalUnit->getParentClass();

    if ($grandParent instanceof ReflectionClass) {
      $parents->addAll($this->_getParentClasses($grandParent));
    }

    return $parents;

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

    // Other can only be instance of an object
    if (!is_object($other)) {
      return false;
    }

    $lcClassName = strtolower($this->className);

    if ($lcClassName == strtolower(get_class($other))) {
      return true;
    }

    $reflection = ReflectionClasses::getReflection($other);

    if (!$reflection instanceof ReflectionClass) {
      return false;
    }

    $interfaces = $reflection->getInterfaces();

    // error_log(
    //   'JEO interfaceCount='.
    //   count($interfaces).
    //   ' target='.
    //   strval($lcClassName),
    // );

    foreach ($interfaces as $interfaceName => $interface) {
      // error_log('JEO interface='.$interfaceName);
      if (strtolower($interfaceName) == $lcClassName) {
        return true;
      }
    }

    $parentClasses = $this->_getParentClasses($reflection);

    foreach ($parentClasses as $parentClass) {
      if (strtolower($parentClass) == $lcClassName) {
        return true;
      }
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
      $this->getExporter()->shortenedExport($other),
      $this->getType(),
      $this->className,
    );
  }

  /**
   * Returns a string representation of the constraint.
   *
   * @return string
   */
  public function toString(): string {
    return
      sprintf('is instance of %s "%s"', $this->getType(), $this->className);
  }

  private function getType(): string {
    try {
      $reflection = ReflectionClasses::getReflection($this->className);
      if ($reflection instanceof ReflectionClass &&
          $reflection->isInterface()) {
        return 'interface';
      }
    } catch (ReflectionException $e) {
    }

    return 'class';
  }
}
