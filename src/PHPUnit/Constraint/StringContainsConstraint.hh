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
 * Constraint that asserts that the string it is evaluated for contains
 * a given string.
 *
 * Uses strpos() to find the position of the string in the input, if not found
 * the evaluation fails.
 *
 * The sub-string is passed in the constructor.
 *
 * @since Class available since Release 3.0.0
 */
class StringContainsConstraint extends ConstraintBase {

    /**
     * @var string
     */
    protected string $string;

    /**
     * @var bool
     */
    protected bool $ignoreCase;

    /**
     * @param string $string
     * @param bool   $ignoreCase
     */
    public function __construct(mixed $string, bool $ignoreCase = false) {

      parent::__construct();

      $this->string     = strval($string);
      $this->ignoreCase = $ignoreCase;

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

      if ($this->ignoreCase) {
        return stripos($other, $this->string) !== false;
      }

      return strpos($other, $this->string) !== false;

    }

    /**
     * Returns a string representation of the constraint.
     *
     * @return string
     */
    public function toString(): string {

      if ($this->ignoreCase) {
        $string = strtolower($this->string);
      } else {
        $string = $this->string;
      }

      return sprintf(
        'contains "%s"',
        $string
      );

    }
}
