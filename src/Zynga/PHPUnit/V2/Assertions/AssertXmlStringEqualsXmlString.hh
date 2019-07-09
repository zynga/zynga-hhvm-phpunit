<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Environment\XML;
use Zynga\PHPUnit\V2\Exceptions\AssertionFailedException;
use Zynga\PHPUnit\V2\Exceptions\XML\LoadException;

class AssertXmlStringEqualsXmlString {

  /**
   * Asserts that two XML documents are equal.
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

    try {

      $expected = XML::load($expectedXml);
      $actual = XML::load($actualXml);

      return $assertions->assertEquals($expected, $actual, $message);

    } catch (LoadException $e) {

      throw new AssertionFailedException($e->getMessage());

      return false;
    }

  }

}
