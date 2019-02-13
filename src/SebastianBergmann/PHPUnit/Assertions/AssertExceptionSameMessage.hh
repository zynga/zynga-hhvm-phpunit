<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;
use \Exception;

class AssertExceptionSameMessage {

  public static function evaluate(
    Assertions $assertions,
    mixed $expected,
    mixed $actual,
    string $message,
  ): bool {

    $expectedMessage = self::getMessageString($expected);
    $actualMessage = self::getMessageString($actual);

    return
      $assertions->assertEquals($expectedMessage, $actualMessage, $message);

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
