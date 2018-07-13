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

/**
 * Asserts whether or not two JSON objects are equal.
 *
 * @since Class available since Release 3.7.0
 */
class JsonMatchesConstraint extends ConstraintBase {

    /**
     * @var string
     */
    protected string $value;

    /**
     * Creates a new constraint.
     *
     * @param string $value
     */
    public function __construct(mixed $value) {
      parent::__construct();
      $this->value = strval($value);
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

      $other = strval($other);

      $decodedOther = json_decode($other);

      if (json_last_error()) {
        return false;
      }

      $decodedValue = json_decode($this->value);

      if (json_last_error()) {
        return false;
      }

      return $decodedOther == $decodedValue;

    }

    /**
     * Returns a string representation of the object.
     *
     * @return string
     */
    public function toString(): string {
      return sprintf(
        'matches JSON string "%s"',
        $this->value
      );
    }

}
