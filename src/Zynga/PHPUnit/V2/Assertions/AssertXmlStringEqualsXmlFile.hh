<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Environment\XML;

class AssertXmlStringEqualsXmlFile {

  /**
   * Asserts that two XML documents are equal.
   *
   * @param string $expectedFile
   * @param string $actualXml
   * @param string $message
   *
   * @since Method available since Release 3.3.0
   */
  public static function evaluate(
    Assertions $assertions,
    string $expectedFile,
    string $actualXml,
    string $message = '',
  ): bool {

    $expected = XML::loadFile($expectedFile);
    $actual = XML::load($actualXml);

    return $assertions->assertEquals($expected, $actual, $message);

  }

}
