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

use PHPUnit\Util\ErrorHandlerUtil;

/**
 * Error handler that converts PHP errors and warnings to exceptions.
 *
 * @since Class available since Release 4.2.0
 */
class RegexUtil {

  public static function pregMatchSafe(string $pattern, string $subject, mixed $matches = null, int $flags = 0, int $offset = 0): int {

    if ( $matches === null || ! is_array($matches) ) {
      $matches = array();
    }

    $handler_terminator = ErrorHandlerUtil::start(E_WARNING);
    $match              = preg_match($pattern, $subject, $matches, $flags, $offset);
    ErrorHandlerUtil::stop(); // cleaning

    return $match;

  }

}
