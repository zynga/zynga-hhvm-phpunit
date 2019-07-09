<?hh // strict

namespace Zynga\PHPUnit\V2\Exceptions;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use Zynga\PHPUnit\V2\Exceptions\Exception;
use Zynga\PHPUnit\V2\Interfaces\SelfDescribingInterface;

/**
 * Thrown when an assertion failed.
 *
 * @since Class available since Release 2.0.0
 */
class AttributeNotFoundException extends Exception
  implements SelfDescribingInterface {
  /**
   * Wrapper for getMessage() which is declared as final.
   *
   * @return string
   */
  public function toString(): string {
    return $this->getMessage();
  }
}
