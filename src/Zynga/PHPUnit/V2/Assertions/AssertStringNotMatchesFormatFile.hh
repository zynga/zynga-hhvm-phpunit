<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use SebastianBergmann\PHPUnit\Constraints\NotConstraint;
use SebastianBergmann\PHPUnit\Constraints\StringMatchesConstraint;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentExceptionFactory;

class AssertStringNotMatchesFormatFile {

  /**
   * Asserts that a string does not match a given format string.
   *
   * @param string $formatFile
   * @param string $string
   * @param string $message
   *
   * @since Method available since Release 3.5.0
   */
  public static function evaluate(
    Assertions $assertions,
    string $formatFile,
    string $string,
    string $message = '',
  ): bool {

    $assertions->assertFileExists($formatFile, $message);

    if (!is_string($string)) {
      throw InvalidArgumentExceptionFactory::factory(2, 'string');
    }

    $stringMatcherConstraint = new StringMatchesConstraint();
    $stringMatcherConstraint->setExpected(file_get_contents($formatFile));

    $constraint = new NotConstraint();
    $constraint->setExpected($stringMatcherConstraint);

    return $assertions->assertThat($string, $constraint, $message);

  }

}
