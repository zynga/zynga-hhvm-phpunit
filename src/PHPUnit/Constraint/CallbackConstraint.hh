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
use PHPUnit\Util\UtilInvalidArgumentHelper;

/**
 * Constraint that evaluates against a specified closure.
 */
class CallbackConstraint extends ConstraintBase {
    private mixed $callback;

    /**
     * @param callable $callback
     *
     * @throws PHPUnit_Exceptions_Exception
     */
    public function __construct(mixed $callback) {

      parent::__construct();

      // --
      // JEO: This is the legacy php variable based callbacks, they are not
      // carried forward into hacklang.
      // --

      /*
      if (!is_callable($callback)) {
        throw UtilInvalidArgumentHelper::factory(
          1,
          'callable'
        );
      }
      */

      $this->callback = $callback;

    }

    /**
     * Evaluates the constraint for parameter $value. Returns true if the
     * constraint is met, false otherwise.
     *
     * @param mixed $other Value or object to evaluate.
     *
     * @return bool
     */
    public function matches(mixed $other): bool {

      // --
      // JEO: This is the legacy php variable based callbacks, they are not
      // carried forward into hacklang.
      // --
      return false;

      /*
      $callback = $this->callback;

      if ( is_callable($callback) ) {
        return call_user_func($callback, $other);
      }

      return false;
      */

    }

    /**
     * Returns a string representation of the constraint.
     *
     * @return string
     */
    public function toString(): string {
      return 'is accepted by specified callback';
    }

}
