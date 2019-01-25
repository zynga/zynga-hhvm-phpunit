<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;

class AssertFileEquals {

  /**
   * Asserts that the contents of one file is equal to the contents of another
   * file.
   *
   * @param string $expected
   * @param string $actual
   * @param string $message
   * @param bool   $canonicalize
   * @param bool   $ignoreCase
   *
   * @since Method available since Release 3.2.14
   */
  public static function evaluate(
    Assertions $assertions,
    string $expected,
    string $actual,
    string $message = '',
    bool $canonicalize = false,
    bool $ignoreCase = false,
  ): bool {

    $assertions->assertFileExists($expected, $message);
    $assertions->assertFileExists($actual, $message);

    return $assertions->assertEquals(
      file_get_contents($expected),
      file_get_contents($actual),
      $message,
      0.0,
      10,
      $canonicalize,
      $ignoreCase,
    );

  }

}
