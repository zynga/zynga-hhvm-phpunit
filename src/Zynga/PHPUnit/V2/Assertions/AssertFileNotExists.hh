<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Constraints\FileExistsConstraint;
use Zynga\PHPUnit\V2\Constraints\NotConstraint;
use Zynga\PHPUnit\V2\Exceptions\InvalidArgumentExceptionFactory;

class AssertFileNotExists {

  /**
   * Asserts that a file does not exist.
   *
   * @param string $filename
   * @param string $message
   *
   * @since Method available since Release 3.0.0
   */
  public static function evaluate(
    Assertions $assertions,
    string $filename,
    string $message = '',
  ): bool {

    if (!is_string($filename)) {
      throw InvalidArgumentExceptionFactory::factory(1, 'string');
    }

    $fileExistsConstraint = new FileExistsConstraint();

    $constraint = new NotConstraint();
    $constraint->setExpected($fileExistsConstraint);

    return $assertions->assertThat($filename, $constraint, $message);

  }

}
