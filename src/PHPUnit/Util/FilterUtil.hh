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

use Zynga\Framework\Environment\SuperGlobals\V1\SuperGlobals;
use PHPUnit\Exceptions\Exception as PHPUnit_Exceptions_Exception;
use PHPUnit\Exceptions\SyntheticError;
use PHPUnit\Util\BlacklistUtil;

use \Exception;

/**
 * Utility class for code filtering.
 *
 * @since Class available since Release 2.0.0
 */
class FilterUtil {

  private static function _convertTraceToVectorMap(Vector<mixed> $trace): Vector<Map<string, mixed>> {
    $trace = Vector {};

    foreach ( $trace as $frame ) {
      if ( is_array($frame) ) {
        $mappedFrame = Map {};
        foreach ( $frame as $key => $val ) {
          if ( $key === 'file' ) {
            $mappedFrame->set($key, strval($val));
          } elseif ( $key === 'line' ) {
            $mappedFrame->set($key, intval($val));
          }
        }
        $trace->add($mappedFrame);
      }
    }

    return $trace;
  }

    /**
     * Filters stack frames from PHPUnit classes.
     *
     * @param Exception $e
     * @param bool      $asString
     *
     * @return string
     */
    public static function getFilteredStacktrace(Exception $e): string {
      $serverVariables = SuperGlobals::getServerAsMap();

      $prefix = false;
      $script = '';

      if ( $serverVariables->containsKey('SCRIPT_NAME') ) {
        $script = realpath($serverVariables->get('SCRIPT_NAME'));
      }

      //JEO: we don't build ourselves intoa  phar file atm.
      //if (defined('__PHPUNIT_PHAR_ROOT__')) {
      //      $prefix = __PHPUNIT_PHAR_ROOT__;
      //}

      $filteredStacktrace = '';

      $eTrace = Vector {};
      $eFile = '';
      $eLine = 0;

      if ($e instanceof SyntheticError) {
        $eTrace = self::_convertTraceToVectorMap($e->getSyntheticTrace());
        $eFile  = $e->getSyntheticFile();
        $eLine  = $e->getSyntheticLine();
      } elseif ($e instanceof PHPUnit_Exceptions_Exception) {
        $eTrace = self::_convertTraceToVectorMap($e->getSerializableTrace());
        $eFile  = $e->getFile();
        $eLine  = $e->getLine();
      } else {
        if ($e->getPrevious() instanceof Exception && $previous = $e->getPrevious()) {
          $eTrace = self::_convertTraceToVectorMap($previous->getTrace());
          $eFile  = $previous->getFile();
          $eLine  = $previous->getLine();
        }

      }

        if ( static::frameExists($eTrace, $eFile, $eLine) !== true) {
          // Prepend the eFile:eLine combo to the stack.
          $eTrace->reverse(); // sort it backwards.
          $eTrace->add(Map {'file' => $eFile, 'line' => $eLine });
          $eTrace->reverse(); // return sorting to the way it was.
        }

        $blacklist = new BlacklistUtil();

        foreach ($eTrace as $frame) {

          var_dump($frame);
          if ( $frame->containsKey('file') ) {

            $file = strval($frame->get('file'));
            $line = -1;

            if ( $frame->containsKey('line') ) {
              $line = intval($frame->get('line'));
            }

            if ( ! is_file($file) ) {
              continue;
            }

            if ( $blacklist->isBlacklisted($file) ) {
              continue;
            }

            if ( $prefix !== false && strpos($file, $prefix) === 0 ) {
              continue;
            }

            if ( $file === $script ) {
              continue;
            }

            $filteredStacktrace .= sprintf(
                "%s:%s\n",
                $file,
                $line != -1 ? $line : '?'
            );

          }

        }

        return $filteredStacktrace;
    }

    /**
     * @param array  $trace
     * @param string $file
     * @param int    $line
     *
     * @return bool
     *
     * @since Method available since Release 3.3.2
     */
    public static function frameExists(Vector<Map<string, mixed>> $trace, string $file, int $line): bool {
        foreach ($trace as $frame) {

          if ( $frame->containsKey('file') === true && $frame->get('file') == $file &&
               $frame->containsKey('line') === true && $frame->get('line') == $line
             ) {
                 return true;
               }

        }

        return false;
    }
}
