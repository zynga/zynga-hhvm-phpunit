<?hh // strict

namespace PHPUnit\Util;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use PHPUnit\Exceptions\Exception as PHPUnit_Exceptions_Exception;

/**
 * Factory for PHPUnit_Exceptions_Exception objects that are used to describe
 * invalid arguments passed to a function or method.
 *
 * @since Class available since Release 3.4.0
 */
class InvalidArgumentHelperUtil {
    /**
     * @param int    $argument
     * @param string $type
     * @param mixed  $value
     *
     * @return PHPUnit_Exceptions_Exception
     */
    public static function factory(int $argument, string $type, mixed $value = null): PHPUnit_Exceptions_Exception {

        $stack = debug_backtrace();

        $stringValue = '';

        if ( $value === null ) {
          $stringValue = '(No Value)';
        } else {
          $stringValue = '(' . gettype($value) . '#' . strval($value) . ')';
        }

        $className      = '';
        $functionName   = '';

        if ( is_array($stack) && array_key_exists(1, $stack) ) {
          $frame = $stack[1];
          if ( is_array($frame) && array_key_exists('class', $frame) && array_key_exists('function', $frame)) {
            $className = $frame['class'];
            $functionName = $frame['function'];
          }
        }

        return new PHPUnit_Exceptions_Exception(
            sprintf(
                'Argument #%d%sof %s::%s() must be a %s',
                $argument,
                $stringValue,
                $className,
                $functionName,
                $type
            )
        );
    }
}
