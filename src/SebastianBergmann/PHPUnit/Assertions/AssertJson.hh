<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;
use SebastianBergmann\PHPUnit\Constraints\IsJsonConstraint;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentExceptionFactory;

class AssertJson {

  /**
   * Asserts that a string is a valid JSON string.
   *
   * @param string $actualJson
   * @param string $message
   *
   * @since Method available since Release 3.7.20
   */
  public static function evaluate(
    Assertions $assertions,
    string $actualJson,
    string $message = '',
  ): bool {

    if (!is_string($actualJson)) {
      throw InvalidArgumentExceptionFactory::factory(1, 'string');
    }

    $constraint = new IsJsonConstraint();

    return $assertions->assertThat($actualJson, $constraint, $message);

  }

}
