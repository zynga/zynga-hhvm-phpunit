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
use PHPUnit\Exceptions\ExpectationFailedException;
use PHPUnit\Util\UtilInvalidArgumentHelper;
use SebastianBergmann\Comparator\ComparisonFailure;
use SebastianBergmann\Comparator\Factory as ComparatorFactory;

/**
 * Constraint that checks if one value is equal to another.
 *
 * Equality is checked with PHP's == operator, the operator is explained in
 * detail at {@url http://www.php.net/manual/en/types.comparisons.php}.
 * Two values are equal if they have the same value disregarding type.
 *
 * The expected value is passed in the constructor.
 *
 * @since Class available since Release 3.0.0
 */
class IsEqualConstraint extends ConstraintBase {

    /**
     * @var mixed
     */
    protected mixed $value;

    /**
     * @var float
     */
    protected float $delta = 0.0;

    /**
     * @var int
     */
    protected int $maxDepth = 10;

    /**
     * @var bool
     */
    protected bool $canonicalize = false;

    /**
     * @var bool
     */
    protected bool $ignoreCase = false;

    /**
     * @var SebastianBergmann\Comparator\ComparisonFailure
     */
    protected ?ComparisonFailure $lastFailure = null;

    /**
     * @param mixed $value
     * @param float $delta
     * @param int   $maxDepth
     * @param bool  $canonicalize
     * @param bool  $ignoreCase
     *
     * @throws PHPUnit_Exceptions_Exception
     */
    public function __construct(mixed $value, float $delta = 0.0, int $maxDepth = 10, bool $canonicalize = false, bool $ignoreCase = false) {

      parent::__construct();

      $this->value        = $value;
      $this->delta        = $delta;
      $this->maxDepth     = $maxDepth;
      $this->canonicalize = $canonicalize;
      $this->ignoreCase   = $ignoreCase;

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
        // If $this->value and $other are identical, they are also equal.
        // This is the most common path and will allow us to skip
        // initialization of all the comparators.
        if ($this->value === $other) {
            return true;
        }

        $comparatorFactory = ComparatorFactory::getInstance();

        try {
          $comparator = $comparatorFactory->getComparatorFor(
            $this->value,
            $other
          );

          $comparator->assertEquals(
            $this->value,
            $other,
            $this->delta,
            $this->canonicalize,
            $this->ignoreCase
          );

        } catch (ComparisonFailure $f) {

          if ($returnResult) {
            return false;
          }

          throw new ExpectationFailedException(
            trim($description . "\n" . $f->getMessage()),
            $f
          );

        }

        return true;
    }

    /**
     * Returns a string representation of the constraint.
     *
     * @return string
     */
    public function toString(): string {
        $delta = '';

        if (is_string($this->value)) {
            if (strpos($this->value, "\n") !== false) {
                return 'is equal to <text>';
            } else {
                return sprintf(
                    'is equal to <string:%s>',
                    $this->value
                );
            }
        } else {
            if ($this->delta != 0) {
                $delta = sprintf(
                    ' with delta <%F>',
                    $this->delta
                );
            }

            return sprintf(
                'is equal to %s%s',
                $this->exporter->export($this->value),
                $delta
            );
        }
    }
}
