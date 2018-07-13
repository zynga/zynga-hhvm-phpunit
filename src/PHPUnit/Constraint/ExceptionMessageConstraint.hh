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
use \Exception;

/**
 * @since Class available since Release 3.6.6
 */
class ExceptionMessageConstraint extends ConstraintBase {

    /**
     * @var string
     */
    protected string $expectedMessage;

    /**
     * @param string $expected
     */
    public function __construct(mixed $expected) {
      parent::__construct();
      $this->expectedMessage = strval($expected);
    }

    /**
     * Evaluates the constraint for parameter $other. Returns true if the
     * constraint is met, false otherwise.
     *
     * @param Exception $other
     *
     * @return bool
     */
    public function matches(mixed $other): bool {
      if ( $other instanceof Exception ) {
        return strpos($other->getMessage(), $this->expectedMessage) !== false;
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
      if ( $other instanceof Exception ) {
        return sprintf(
            "exception message '%s' contains '%s'",
            $other->getMessage(),
            $this->expectedMessage
        );
      }
      return '';
    }

    /**
     * @return string
     */
    public function toString(): string {
      return 'exception message contains ';
    }
}
