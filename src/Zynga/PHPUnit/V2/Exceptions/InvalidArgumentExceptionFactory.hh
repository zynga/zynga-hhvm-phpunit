<?hh // strict

namespace Zynga\PHPUnit\V2\Exceptions;

use Zynga\PHPUnit\V2\Exceptions\InvalidArgumentException;

class InvalidArgumentExceptionFactory {

  public static function factory(
    int $argument,
    string $type,
    mixed $value = null,
  ): InvalidArgumentException {

    $stack = debug_backtrace();

    return new InvalidArgumentException(
      sprintf(
        'Argument #%d%sof %s::%s() must be a %s',
        $argument,
        $value !== null
          ? ' ('.gettype($value).'#'.strval($value).')'
          : ' (No Value) ',
        $stack[1]['class'],
        $stack[1]['function'],
        $type,
      ),
    );

  }

}
