<?hh // strict

namespace SebastianBergmann\PHPUnit\Exceptions;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use SebastianBergmann\PHPUnit\Exceptions\Exception;

/**
 * Thrown when an there is a warning.
 *
 * @since Class available since Release 5.0.0
 */
class WarningException extends Exception {

  /**
   * Wrapper for getMessage() which is declared as final.
   *
   * @return string
   */
  public function toString(): string {
    return $this->getMessage();
  }

}
