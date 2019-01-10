<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;
use SebastianBergmann\PHPUnit\Constraints\ArrayHasKeyConstraint;
use SebastianBergmann\PHPUnit\Constraints\NotConstraint;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentExceptionFactory;

use \ArrayAccess;

class AssertArrayNotHasKey {
  /**
   * Asserts that an array does not have a specified key.
   *
   * @param mixed             $key
   * @param array|ArrayAccess $array
   * @param string            $message
   *
   * @since Method available since Release 3.0.0
   */
  final static public function evaluate(
    Assertions $assertions,
    mixed $key,
    mixed $array,
    string $message = '',
  ): bool {

    if (!(is_int($key) || is_string($key))) {
      throw InvalidArgumentExceptionFactory::factory(1, 'integer or string');
    }

    if (!(is_array($array) || $array instanceof ArrayAccess)) {
      throw InvalidArgumentExceptionFactory::factory(
        2,
        'array or ArrayAccess',
      );
    }

    $hasKeyConstraint = new ArrayHasKeyConstraint();
    $hasKeyConstraint->setExpected($key);

    $notConstraint = new NotConstraint();
    $notConstraint->setExpected($hasKeyConstraint);

    return $assertions->assertThat($array, $notConstraint, $message);

  }

}
