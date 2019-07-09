<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Constraints\JsonMatchesConstraint;
use Zynga\PHPUnit\V2\Constraints\NotConstraint;

class AssertJsonStringNotEqualsJsonFile {

  /**
   * Asserts that the generated JSON encoded object and the content of the given file are not equal.
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

    $notConstraint = new NotConstraint();
    $notConstraint->setExpected($constraint);

    return $assertions->assertThat($actualJson, $notConstraint, $message);

  }

}
