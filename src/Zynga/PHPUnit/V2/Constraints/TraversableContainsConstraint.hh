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
use \SplObjectStorage;

/**
 * Constraint that asserts that the Traversable it is applied to contains
 * a given value.
 *
 * @since Class available since Release 3.0.0
 */
class TraversableContainsConstraint extends Base {
  /**
   * @var bool
   */
  private bool $checkForObjectIdentity = true;

  /**
   * @var bool
   */
  private bool $checkForNonObjectIdentity = false;

  /**
   * @var mixed
   */
  private mixed $value = null;

  public function setCheckForObjectIdentity(bool $value): bool {
    $this->checkForObjectIdentity = $value;
    return true;
  }

  public function setCheckForNonObjectIdentity(bool $value): bool {
    $this->checkForNonObjectIdentity = $value;
    return true;
  }

  public function setExpected(mixed $expected): bool {
    $this->value = $expected;
    return true;
  }

  public function resetExpected(): bool {
    $this->value = null;
    return true;
  }

  private function _matchesForArrayOther(bool $checkVar, mixed $other): bool {
    if (is_array($other)) {
      foreach ($other as $element) {
        if ($checkVar && $element === $this->value) {
          return true;
        } else if (!$checkVar &&
                   $element == $this->value) {
          return true;
        }
      }
    }
    return false;
  }

  private function _matchesForTraversableOther(bool $checkVar, mixed $other): bool {
    if ($other instanceof Traversable) {
      foreach ($other as $element) {
        if ($checkVar && $element === $this->value) {
          return true;
        } else if (!$checkVar &&
                   $element == $this->value) {
          return true;
        }
      }
    }
    return false;
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

    if ($other instanceof SplObjectStorage) {
      return $other->contains($this->value);
    }

    if (is_object($this->value)) {

      if (is_array($other)) {
        return $this->_matchesForArrayOther($this->checkForObjectIdentity, $other);
        // foreach ($other as $element) {
        //   if ($this->checkForObjectIdentity && $element === $this->value) {
        //     return true;
        //   } else if (!$this->checkForObjectIdentity &&
        //              $element == $this->value) {
        //     return true;
        //   }
        // }
        // return false;
      } else if ($other instanceof Traversable) {
        return $this->_matchesForTraversableOther($this->checkForObjectIdentity, $other);
        // foreach ($other as $element) {
        //   if ($this->checkForObjectIdentity && $element === $this->value) {
        //     return true;
        //   } else if (!$this->checkForObjectIdentity &&
        //              $element == $this->value) {
        //     return true;
        //   }
        // }
        // return false;
      }

      return false;

    } else if (is_array($other)) {
      return $this->_matchesForArrayOther($this->checkForNonObjectIdentity, $other);
      // foreach ($other as $element) {
      //   if ($this->checkForNonObjectIdentity && $element === $this->value) {
      //     return true;
      //   } else if (!$this->checkForNonObjectIdentity &&
      //              $element == $this->value) {
      //     return true;
      //   }
      // }
    } else if ($other instanceof Traversable) {
      return $this->_matchesForTraversableOther($this->checkForNonObjectIdentity, $other);
      // foreach ($other as $element) {
      //   if ($this->checkForNonObjectIdentity && $element === $this->value) {
      //     return true;
      //   } else if (!$this->checkForNonObjectIdentity &&
      //              $element == $this->value) {
      //     return true;
      //   }
      // }
    }

    return false;
  }

  /**
   * Returns a string representation of the constraint.
   *
   * @return string
   */
  public function toString(): string {
    if (is_string($this->value)) {
      if (strpos($this->value, "\n") === false) {
        return 'contains "'.strval($this->value).'"';
      }
    }
    return 'contains '.$this->getExporter()->export($this->value);
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
      '%s %s',
      is_array($other) ? 'an array' : 'a traversable',
      $this->toString(),
    );
  }
}
