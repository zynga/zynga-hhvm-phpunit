<?hh // strict

namespace Zynga\PHPUnit\V2\Assertions;

use Zynga\PHPUnit\V2\Assertions;
use Zynga\PHPUnit\V2\Exceptions\AssertionFailedException;
use
  Zynga\Framework\Environment\ErrorCapture\V1\Handler\ConvertErrorsToExceptions
;
use \Exception;
use \RuntimeException;

class AssertExceptionRegexpMessage {

  public static function evaluate(
    Assertions $assertions,
    string $expected,
    mixed $actual,
    string $message,
  ): bool {

    // Is the regex valid
    $capture = new ConvertErrorsToExceptions();
    $e = null;

    try {

      $capture->start();

      preg_match($expected, null);

      $capture->stop();

    } catch (Exception $e) {

      throw new AssertionFailedException(
        "Invalid regex provided: ".$e->getMessage(),
      );
    }

    $actualMessage = self::getMessageString($actual);

    return $assertions->assertRegExp($expected, $actualMessage, $message);

  }

  private static function getMessageString(mixed $object): string {

    if (is_string($object)) {
      return $object;
    }

    if ($object instanceof Exception) {
      return $object->getMessage();
    }

    return '';

  }

}
