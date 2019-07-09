<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Constraints\JsonMatchesConstraint;

class AssertJsonFileEqualsJsonFile {

  /**
   * Asserts that two JSON files are equal.
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

    $constraintActual = new JsonMatchesConstraint();
    $constraintActual->setExpected($actualJson);

    // --
    // JEO: Not quite sure why we are running this twice, as the first matcher
    // should be enough. Possible performance improvement, but skipping for
    // initial port.
    // --
    $assertions->assertThat($expectedJson, $constraintActual, $message);

    return
      $assertions->assertThat($actualJson, $constraintExpected, $message);

  }

}
