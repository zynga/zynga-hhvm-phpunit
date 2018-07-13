<?hh // strict

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace PHPUnit\Exceptions;

use PHPUnit\Exceptions\Exception as PHPUnit_Exceptions_Exception;
use PHPUnit\Interfaces\SelfDescribingInterface;

/**
 * Thrown when an assertion failed.
 *
 * @since Class available since Release 2.0.0
 */
class AssertionFailedError extends PHPUnit_Exceptions_Exception implements SelfDescribingInterface {
  /**
   * Wrapper for getMessage() which is declared as final.
   *
   * @return string
   */
  public function toString(): string {
    return $this->getMessage();
  }
}
