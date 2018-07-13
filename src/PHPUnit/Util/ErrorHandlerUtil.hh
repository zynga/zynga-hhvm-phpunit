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

use Zynga\Framework\Environment\ErrorCapture\V1\Handler\ErrorCollector;
use Zynga\Framework\Environment\ErrorCapture\V1\CapturedError;

use PHPUnit\Exceptions\Error;
use PHPUnit\Exceptions\Error\DeprecatedError;
use PHPUnit\Exceptions\Error\NoticeError;
use PHPUnit\Exceptions\Error\WarningError;
use PHPUnit\Exceptions\Error\UnknownError;

/**
 * Error handler that converts PHP errors and warnings to exceptions.
 *
 * @since Class available since Release 3.3.0
 */
class ErrorHandlerUtil {

  protected static ?ErrorCollector $collector = null;

  /**
   * Returns the error stack.
   *
   * @return array
   */
  public static function getErrorStack(): Vector<CapturedError> {
    if ( self::$collector instanceof ErrorCollector ) {
      return self::$collector->getErrors();;
    }
    return Vector {};
  }

  public static function initCollector(): void {

    if ( self::$collector === null ) {
      self::$collector = new ErrorCollector();
    }

  }


  public static function stop(): bool {

    self::initCollector();

    $collector = self::$collector;

    if ( $collector instanceof ErrorCollector ) {

      $collector->stop();

      $errors = $collector->getErrors();

      foreach ($errors as $error ) {

        if (NoticeError::$enabled === true && $collector->isErrorNumberNotice($error->errno) ) {
          throw new NoticeError($error->errstr, $error->errno, $error->errfile, $error->errline);
        } elseif (WarningError::$enabled === true && $collector->isErrorNumberWarning($error->errno) ) {
          throw new WarningError($error->errstr, $error->errno, $error->errfile, $error->errline);
        } elseif (DeprecatedError::$enabled === true && $collector->isErrorNumberDeprecated($error->errno) ) {
          throw new DeprecatedError($error->errstr, $error->errno, $error->errfile, $error->errline);
        }

      }

      $collector->clear();

    }

    return true;

  }

    /**
     * Registers an error handler and returns a function that will restore
     * the previous handler when invoked
     *
     * @param int $severity PHP predefined error constant
     *
     * @throws Exception if event of specified severity is emitted
     */
    public static function start(int $severity = E_WARNING): bool {

      self::initCollector();

      $collector = self::$collector;

      if ( $collector instanceof ErrorCollector ) {

        $collector->setupViaSeverity($severity);

        $collector->start();;

        return true;

      }

      return false;

    }
}
