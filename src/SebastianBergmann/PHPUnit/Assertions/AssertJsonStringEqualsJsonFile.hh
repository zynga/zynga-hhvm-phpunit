<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;
use SebastianBergmann\PHPUnit\Constraints\JsonMatchesConstraint;

class AssertJsonStringEqualsJsonFile {

  /**
   * Asserts that the generated JSON encoded object and the content of the given file are equal.
   *
   * @param string $expectedFile
   * @param string $actualJson
   * @param string $message
   */
  public static function evaluate(
    Assertions $assertions,
    string $expectedFile,
    string $actualJson,
    string $message = '',
  ): bool {

    $assertions->assertFileExists($expectedFile, $message);
    $expectedJson = file_get_contents($expectedFile);

    $assertions->assertJson($expectedJson, $message);
    $assertions->assertJson($actualJson, $message);

    // call constraint
    $constraint = new JsonMatchesConstraint();
    $constraint->setExpected($expectedJson);

    return $assertions->assertThat($actualJson, $constraint, $message);

  }

}
