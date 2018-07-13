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

use PHPUnit\Constraint\CompositeConstraint;
use PHPUnit\Exceptions\ExpectationFailedException;
use PHPUnit\Interfaces\ConstraintInterface;

use \PHPUnit_Framework_Assert;

/**
 * @since Class available since Release 3.1.0
 */
class AttributeConstraint extends CompositeConstraint {
    /**
     * @var string
     */
    protected string $attributeName;

    protected PHPUnit_Framework_Assert $assertObject;

    /**
     * @param PHPUnit_Framework_Constraint $constraint
     * @param string                       $attributeName
     */
    public function __construct(ConstraintInterface $constraint, string $attributeName, PHPUnit_Framework_Assert $assertObject) {

      parent::__construct($constraint);

      $this->attributeName = $attributeName;
      $this->assertObject = $assertObject;

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
      return parent::evaluate(
        $this->assertObject->readAttribute(
          $other,
          $this->attributeName
        ),
        $description,
        $returnResult
      );
    }

    /**
     * Returns a string representation of the constraint.
     *
     * @return string
     */
    public function toString(): string {
      return 'attribute "' . $this->attributeName . '" ' .
        $this->innerConstraint->toString();
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
      return $this->toString();
    }

}
