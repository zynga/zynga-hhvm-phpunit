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

/**
 * Constraint that asserts that the value it is evaluated for is of a
 * specified type.
 *
 * The expected value is passed in the constructor.
 *
 * @since Class available since Release 3.0.0
 */
class IsTypeConstraint extends ConstraintBase {
    const TYPE_ARRAY    = 'array';
    const TYPE_BOOL     = 'bool';
    const TYPE_FLOAT    = 'float';
    const TYPE_INT      = 'int';
    const TYPE_NULL     = 'null';
    const TYPE_NUMERIC  = 'numeric';
    const TYPE_OBJECT   = 'object';
    const TYPE_RESOURCE = 'resource';
    const TYPE_STRING   = 'string';
    const TYPE_SCALAR   = 'scalar';
    const TYPE_CALLABLE = 'callable';

    /**
     * @var array
     */
    protected Map<string, bool> $types = Map {
        'array'    => true,
        'boolean'  => true,
        'bool'     => true,
        'double'   => true,
        'float'    => true,
        'integer'  => true,
        'int'      => true,
        'null'     => true,
        'numeric'  => true,
        'object'   => true,
        'real'     => true,
        'resource' => true,
        'string'   => true,
        'scalar'   => true,
        'callable' => true
    };

    /**
     * @var string
     */
    protected mixed $type;

    /**
     * @param string $type
     *
     * @throws PHPUnit_Exceptions_Exception
     */
    public function __construct(mixed $type) {

      parent::__construct();

      if ( ! $this->types->containsKey($type) ) {
        throw new PHPUnit_Exceptions_Exception(
          sprintf(
            'Type specified for PHPUnit_Framework_Constraint_IsType <%s> ' .
            'is not a valid type.',
            $type
          )
        );
      }

      $this->type = $type;

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

      if ( is_string($other) !== true ) {
        return false;
      }

      $localType = strval($this->type);

      if ( is_string($localType) !== true ) {
        return false;
      }

      switch ($localType) {
        case 'numeric':
          return is_numeric($other);
        case 'integer':
          return is_int($other);
        case 'int':
          return is_int($other);
        case 'double':
          return is_float($other);
        case 'float':
          return is_float($other);
        case 'real':
          return is_float($other);
        case 'string':
          return is_string($other);
        case 'boolean':
          return is_bool($other);
        case 'bool':
          return is_bool($other);
        case 'null':
          return is_null($other);
        case 'array':
          return is_array($other);
        case 'object':
          return is_object($other);
        case 'resource':
          return is_resource($other) || is_string(@get_resource_type($other));
        case 'scalar':
          return is_scalar($other);
        case 'callable':
          return is_callable($other);
      }

    }

    /**
     * Returns a string representation of the constraint.
     *
     * @return string
     */
    public function toString(): string {
      return sprintf('is of type "%s"', strval($this->type));
    }

}
