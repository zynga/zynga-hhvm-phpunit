<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;

class AssertJsonStringEqualsJsonString {

  /**
   * Asserts that two given JSON encoded objects or arrays are equal.
   *
   * @param string $expectedJson
   * @param string $actualJson
   * @param string $message
   */
  public static function evaluate(
    Assertions $assertions,
    string $expectedJson,
    string $actualJson,
    string $message = '',
  ): bool {

    $assertions->assertJson($expectedJson, $message);
    $assertions->assertJson($actualJson, $message);

    $expected = json_decode($expectedJson);
    $actual = json_decode($actualJson);

    return $assertions->assertEquals($expected, $actual, $message);

  }

}
