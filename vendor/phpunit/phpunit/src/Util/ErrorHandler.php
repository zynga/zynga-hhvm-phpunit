<?hh // partial

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use PHPUnit\Exceptions\Error;
use PHPUnit\Exceptions\Error\Deprecated as Error_Deprecated;
use PHPUnit\Exceptions\Error\Notice as Error_Notice;
use PHPUnit\Exceptions\Error\Warning as Error_Warning;

/**
 * Error handler that converts PHP errors and warnings to exceptions.
 *
 * @since Class available since Release 3.3.0
 */
class PHPUnit_Util_ErrorHandler
{
    protected static $errorStack = [];

    /**
     * Returns the error stack.
     *
     * @return array
     */
    public static function getErrorStack()
    {
        return self::$errorStack;
    }

    /**
     * @param int    $errno
     * @param string $errstr
     * @param string $errfile
     * @param int    $errline
     *
     * @throws Error
     */
    public static function handleError($errno, $errstr, $errfile, $errline) {
        if (!($errno & error_reporting())) {
            return false;
        }

        self::$errorStack[] = [$errno, $errstr, $errfile, $errline];

        $trace = debug_backtrace();

        array_shift($trace);

        foreach ($trace as $frame) {
            if ($frame['function'] == '__toString') {
                return false;
            }
        }

        if ($errno == E_NOTICE || $errno == E_USER_NOTICE || $errno == E_STRICT) {

            if (Error_Notice::$enabled !== true) {
                return false;
            }

            throw new Error_Notice($errstr, $errno, $errfile, $errline);

        } elseif ($errno == E_WARNING || $errno == E_USER_WARNING) {

            if (Error_Warning::$enabled !== true) {
                return false;
            }

            throw new Error_Warning($errstr, $errno, $errfile, $errline);

        } elseif ($errno == E_DEPRECATED || $errno == E_USER_DEPRECATED) {

            if (Error_Deprecated::$enabled !== true) {
                return false;
            }

            throw new Error_Deprecated($errstr, $errno, $errfile, $errline);
        }

        throw new Error($errstr, $errno, $errfile, $errline);

    }

    /**
     * Registers an error handler and returns a function that will restore
     * the previous handler when invoked
     *
     * @param int $severity PHP predefined error constant
     *
     * @throws Exception if event of specified severity is emitted
     */
    public static function handleErrorOnce($severity = E_WARNING)
    {
        $terminator = function () {
            static $expired = false;
            if (!$expired) {
                $expired = true;
                // cleans temporary error handler
                return restore_error_handler();
            }
        };

        set_error_handler(function ($errno, $errstr) use ($severity) {
            if ($errno === $severity) {
                return;
            }

            return false;
        });

        return $terminator;
    }
}
