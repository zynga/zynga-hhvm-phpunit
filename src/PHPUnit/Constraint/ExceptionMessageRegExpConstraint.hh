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
use PHPUnit\Util\RegexUtil;
use \Exception;

/**
 * @since Class available since Release 4.3.0
 */
class ExceptionMessageRegExpConstraint extends ConstraintBase {

    /**
     * @var string
     */
    protected string $expectedMessageRegExp;

    /**
     * @param string $expected
     */
    public function __construct(mixed $expected) {
      parent::__construct();
      $this->expectedMessageRegExp = strval($expected);
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
        $match = RegexUtil::pregMatchSafe($this->expectedMessageRegExp, $other->getMessage());

        if (1 === $match) {
          return true;
        }

        throw new PHPUnit_Exceptions_Exception(
          "Invalid expected exception message regex given: '{$this->expectedMessageRegExp}'"
        );

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
            "exception message '%s' matches '%s'",
            $other->getMessage(),
            $this->expectedMessageRegExp
        );
      }
      return '';
    }

    /**
     * @return string
     */
    public function toString(): string {
        return 'exception message matches ';
    }
}
