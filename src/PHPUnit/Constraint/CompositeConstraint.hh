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
use PHPUnit\Exceptions\ExpectationFailedException;
use PHPUnit\Interfaces\ConstraintInterface;

/**
 * @since Class available since Release 3.1.0
 */
abstract class CompositeConstraint extends ConstraintBase {

    /**
     * @var PHPUnit_Framework_Constraint
     */
    protected ConstraintInterface $innerConstraint;

    /**
     * @param PHPUnit_Framework_Constraint $innerConstraint
     */
    public function __construct(ConstraintInterface $innerConstraint) {
        parent::__construct();
        $this->innerConstraint = $innerConstraint;
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
      try {
        return $this->innerConstraint->evaluate(
          $other,
          $description,
          $returnResult
        );
      } catch (ExpectationFailedException $e) {
        $this->fail($other, $description);
      }
    }

    /**
     * Counts the number of constraint elements.
     *
     * @return int
     */
    public function count(): int {
        return count($this->innerConstraint);
    }
    
}
