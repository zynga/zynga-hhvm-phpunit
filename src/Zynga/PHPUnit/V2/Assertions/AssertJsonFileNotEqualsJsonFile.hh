<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use SebastianBergmann\PHPUnit\Constraints\JsonMatchesConstraint;
use SebastianBergmann\PHPUnit\Constraints\NotConstraint;

class AssertJsonFileNotEqualsJsonFile {

  /**
   * Asserts that two JSON files are not equal.
   *
   * @param string $expectedFile
   * @param string $actualFile
   * @param string $message
   */
  public static function evaluate(
    Assertions $assertions,
    string $expectedFile,
    string $actualFile,
    string $message = '',
  ): bool {

    $assertions->assertFileExists($expectedFile, $message);
    $assertions->assertFileExists($actualFile, $message);

    $actualJson = file_get_contents($actualFile);
    $expectedJson = file_get_contents($expectedFile);

    $assertions->assertJson($expectedJson, $message);
    $assertions->assertJson($actualJson, $message);

    // call constraint
    $constraintExpected = new JsonMatchesConstraint();
    $constraintExpected->setExpected($expectedJson);

    $notConstraintExpected = new NotConstraint();
    $notConstraintExpected->setExpected($constraintExpected);

    return $assertions->assertThat($actualJson, $notConstraintExpected, $message);

  }

}
