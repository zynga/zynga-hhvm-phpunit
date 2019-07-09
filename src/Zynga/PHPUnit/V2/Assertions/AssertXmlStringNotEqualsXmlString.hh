<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Environment\XML;

class AssertXmlStringNotEqualsXmlString {

  /**
   * Asserts that two XML documents are not equal.
   *
   * @param string $expectedXml
   * @param string $actualXml
   * @param string $message
   *
   * @since Method available since Release 3.1.0
   */
  public static function evaluate(
    Assertions $assertions,
    string $expectedXml,
    string $actualXml,
    string $message = '',
  ): bool {

    $expected = XML::load($expectedXml);
    $actual = XML::load($actualXml);

    return $assertions->assertNotEquals($expected, $actual, $message);

  }

}
