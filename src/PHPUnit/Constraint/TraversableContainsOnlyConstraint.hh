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
use PHPUnit\Constraint\IsInstanceOfConstraint;
use PHPUnit\Constraint\IsTypeConstraint;

use PHPUnit\Exceptions\ExpectationFailedException;
use PHPUnit\Interfaces\ConstraintInterface;

/**
 * Constraint that asserts that the Traversable it is applied to contains
 * only values of a given type.
 *
 * @since Class available since Release 3.1.4
 */
class TraversableContainsOnlyConstraint extends ConstraintBase {

    /**
     * @var PHPUnit_Framework_Constraint
     */
    protected ConstraintInterface $constraint;

    /**
     * @var string
     */
    protected mixed $type;

    /**
     * @param string $type
     * @param bool   $isNativeType
     */
    public function __construct(mixed $type, bool $isNativeType = true) {

      parent::__construct();

      if ($isNativeType) {
        $this->constraint = new IsTypeConstraint($type);
      } else {
        $this->constraint = new IsInstanceOfConstraint($type);
      }

      $this->type = $type;

    }

    /**
     * Evaluates the constraint for parameter $other
     *
     * If $returnResult is set to false (the default), an exception is thrown
     * in case of a failure. null is returned otherwise.
     *
     * If $returnResult is true, the result of the evaluation is returned as
     * a boolean value instead: true in case of success, false in case of a
     * failure.
     *
     * @param mixed  $other        Value or object to evaluate.
     * @param string $description  Additional information about the test
     * @param bool   $returnResult Whether to return a result or throw an exception
     *
     * @return mixed
     *
     * @throws ExpectationFailedException
     */
    public function evaluate(mixed $other, string $description = '', bool $returnResult = false): mixed {

      $success = true;

      if ( $other instanceof Traversable ) {

        foreach ($other as $item) {
          if (!$this->constraint->evaluate($item, '', true)) {
            $success = false;
            break;
          }
        }

      }

      if ($returnResult) {
        return $success;
      }

      if (!$success) {
        $this->fail($other, $description);
      }

    }

    /**
     * Returns a string representation of the constraint.
     *
     * @return string
     */
    public function toString(): string {
      return 'contains only values of type "' . strval($this->type) . '"';
    }

}
