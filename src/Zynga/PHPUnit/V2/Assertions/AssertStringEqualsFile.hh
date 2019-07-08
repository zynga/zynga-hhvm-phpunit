<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;

class AssertStringEqualsFile {

  /**
   * Asserts that the contents of a string is equal
   * to the contents of a file.
   *
   * @param string $expectedFile
   * @param string $actualString
   * @param string $message
   * @param bool   $canonicalize
   * @param bool   $ignoreCase
   *
   * @since Method available since Release 3.3.0
   */
  public static function evaluate(
    Assertions $assertions,
    string $expectedFile,
    string $actualString,
    string $message = '',
    bool $canonicalize = false,
    bool $ignoreCase = false,
  ): bool {

    $assertions->assertFileExists($expectedFile, $message);

    return $assertions->assertEquals(
      file_get_contents($expectedFile),
      $actualString,
      $message,
      0.0,
      10,
      $canonicalize,
      $ignoreCase,
    );

  }

}
