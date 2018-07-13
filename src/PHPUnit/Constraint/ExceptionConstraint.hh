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
use PHPUnit\Util\FilterUtil;
use \Exception;
/**
 * @since Class available since Release 3.6.6
 */
class ExceptionConstraint extends ConstraintBase {

    /**
     * @var string
     */
    protected string $className;

    /**
     * @param string $className
     */
    public function __construct(mixed $className) {
      parent::__construct();
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
      return get_class($other) == $this->className;
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

      if ($other !== null) {
        $message = '';

        if ($other instanceof Exception) {
          $message = '. Message was: "' . $other->getMessage() . '" at'
            . "\n" . FilterUtil::getFilteredStacktrace($other);
        }

        return sprintf(
          'exception of type "%s" matches expected exception "%s"%s',
          get_class($other),
          $this->className,
          $message
        );
      }

      return sprintf(
        'exception of type "%s" is thrown',
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
            'exception of type "%s"',
            $this->className
        );
    }

}
