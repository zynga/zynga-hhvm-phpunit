<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Constraints\StringMatchesConstraint;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentExceptionFactory;

class AssertStringMatchesFormatFile {

  /**
   * Asserts that a string matches a given format file.
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

    $constraint = new StringMatchesConstraint();
    $constraint->setExpected(file_get_contents($formatFile));

    return $assertions->assertThat($string, $constraint, $message);

  }

}
