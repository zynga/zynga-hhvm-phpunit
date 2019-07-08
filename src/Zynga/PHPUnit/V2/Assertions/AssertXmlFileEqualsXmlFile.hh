<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use SebastianBergmann\PHPUnit\Environment\XML;

class AssertXmlFileEqualsXmlFile {

  /**
   * Asserts that two XML files are equal.
   *
   * @param string $expectedFile
   * @param string $actualFile
   * @param string $message
   *
   * @since Method available since Release 3.1.0
   */
  public static function evaluate(
    Assertions $assertions,
    string $expectedFile,
    string $actualFile,
    string $message = '',
  ): bool {

    $expected = XML::loadFile($expectedFile);
    $actual = XML::loadFile($actualFile);

    return $assertions->assertEquals($expected, $actual, $message);

  }

}
