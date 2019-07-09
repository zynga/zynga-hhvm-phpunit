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
use Zynga\PHPUnit\V2\Exceptions\AssertionFailedException;
use Zynga\PHPUnit\V2\Interfaces\ConstraintInterface;
use \Exception;

/**
 * Constraint that asserts that the Traversable it is applied to contains
 * only values of a given type.
 *
 * @since Class available since Release 3.1.4
 */
class TraversableContainsOnlyConstraint extends Base {

  private string $type = '';
  private bool $isNativeType = true;

  public function setExpected(mixed $expected): bool {

    if (is_string($expected)) {
      $this->type = $expected;
      return true;
    }

    throw new Exception(
      'TraversableContainsOnlyConstraint requires either a string name of a native type or class name',
    );

  }

  public function resetExpected(): bool {
    $this->type = '';
    return true;
  }

  public function setIsNativeType(bool $value): bool {
    $this->isNativeType = $value;
    return true;
  }

  /**
   * Evaluates the constraint for parameter $other. Returns true if the
   * constraint is met, false otherwise.
   *
   * This method can be overridden to implement the evaluation algorithm.
   *
   * @param mixed $other Value or object to evaluate.
   *
   * @return bool
   */
  public function matches(mixed $other): bool {

    $constraint = null;

    if ($this->isNativeType == true) {
      $constraint = new IsNativeTypeConstraint();
      $constraint->setExpected($this->type);
    } else {
      $constraint = new IsInstanceOfConstraint();
      $constraint->setExpected($this->type);
    }

    if (is_array($other)) {
      try {
        foreach ($other as $item) {
          if ($constraint->evaluate(
                $item,
                'expected='.$this->type.' found='.get_class($item),
              ) !==
              true) {
            return false;
          }
        }
        return true;
      } catch (AssertionFailedException $e) {
        throw $e;
      }
    } else if ($other instanceof Vector) {
      try {
        foreach ($other as $item) {
          if ($constraint->evaluate(
                $item,
                'expected='.$this->type.' found='.get_class($item),
              ) !==
              true) {
            return false;
          }
        }
        return true;
      } catch (AssertionFailedException $e) {
        throw $e;
      }
    }

    return false;

  }

  /**
   * Returns a string representation of the constraint.
   *
   * @return string
   */
  public function toString(): string {
    return 'contains only values of type "'.$this->type.'"';
  }

}
