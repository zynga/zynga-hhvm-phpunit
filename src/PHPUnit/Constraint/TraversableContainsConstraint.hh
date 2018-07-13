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
use PHPUnit\Exceptions\Exception as PHPUnit_Exceptions_Exception;
use PHPUnit\Util\InvalidArgumentHelperUtil;
use \SplObjectStorage;

/**
 * Constraint that asserts that the Traversable it is applied to contains
 * a given value.
 *
 * @since Class available since Release 3.0.0
 */
class TraversableContainsConstraint extends ConstraintBase {

    /**
     * @var bool
     */
    protected bool $checkForObjectIdentity;

    /**
     * @var bool
     */
    protected bool $checkForNonObjectIdentity;

    /**
     * @var mixed
     */
    protected mixed $value;

    /**
     * @param mixed $value
     * @param bool  $checkForObjectIdentity
     * @param bool  $checkForNonObjectIdentity
     *
     * @throws PHPUnit_Exceptions_Exception
     */
    public function __construct(mixed $value, bool $checkForObjectIdentity = true, bool $checkForNonObjectIdentity = false) {

      parent::__construct();

      $this->checkForObjectIdentity    = $checkForObjectIdentity;
      $this->checkForNonObjectIdentity = $checkForNonObjectIdentity;
      $this->value                     = $value;

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

      if (! $other instanceof Traversable) {
        return false;
      }

      if ( is_object($this->value) ) {
        foreach ($other as $element) {
          if ($this->checkForObjectIdentity && $element === $this->value) {
            return true;
          } elseif (!$this->checkForObjectIdentity && $element == $this->value) {
            return true;
          }
        }
      } else {
        foreach ($other as $element) {
          if ($this->checkForNonObjectIdentity && $element === $this->value) {
            return true;
          } elseif (!$this->checkForNonObjectIdentity && $element == $this->value) {
            return true;
          }
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

      $value = $this->value;

      if (is_string($value) && strpos($value, "\n") !== false) {
        return 'contains "' . $value . '"';
      } else {
        return 'contains ' . $this->exporter->export($value);
      }

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
        $this->toString()
      );

    }
}
