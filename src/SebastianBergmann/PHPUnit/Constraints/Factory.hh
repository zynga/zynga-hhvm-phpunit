<?hh // strict

namespace SebastianBergmann\PHPUnit\Constraints;

use SebastianBergmann\PHPUnit\Interfaces\ConstraintInterface;
use SebastianBergmann\PHPUnit\Constraints\ArrayHasKeyConstraint;
use SebastianBergmann\PHPUnit\Constraints\ArraySubsetConstraint;
use SebastianBergmann\PHPUnit\Constraints\NotConstraint;
use SebastianBergmann\PHPUnit\Constraints\StringContainsConstraint;
use SebastianBergmann\PHPUnit\Constraints\TraversableContainsConstraint;

//use SebastianBergmann\PHPUnit\Constraints\NotConstraint;
use \Exception;

class Factory {

  // --
  // JEO: To be clear, we cannot cache constraints as some of the constraint
  //   use cases will include chaining them together to create meta constraints
  //   therefor needing 2 of the same type of constraint.
  // --
  public static function factory(string $name): ConstraintInterface {

    $constraint = self::getConstraintByName($name);

    if ($constraint instanceof ConstraintInterface) {
      return $constraint;
    }

    throw new Exception('Unable to find constraint given name='.$name);

  }

  private static function getConstraintByName(
    string $name,
  ): ?ConstraintInterface {

    if ($name == 'ArrayHasKey') {
      return new ArrayHasKeyConstraint();
    } else if ($name == 'ArraySubset') {
      return new ArraySubsetConstraint();
    } else if ($name == 'StringContains') {
      return new StringContainsConstraint();
    } else if ($name == 'TraversableContains') {
      return new TraversableContainsConstraint();
    } else if ($name == 'Not') {
      return new NotConstraint();
    }

    //if ($name == 'IsTrue') {
    //  error_log('IsTrue');
    //}

    return null;

  }

}
