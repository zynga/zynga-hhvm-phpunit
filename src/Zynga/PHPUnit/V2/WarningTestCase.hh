<?hh // strict

namespace Zynga\PHPUnit\V2;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use Zynga\PHPUnit\V2\Exceptions\WarningException;
use Zynga\PHPUnit\V2\TestCase;

/**
 * A warning.
 *
 * @since Class available since Release 2.0.0
 */
class WarningTestCase extends TestCase {
  private string $_message = '';

  /**
   * @param string $message
   */
  public function __construct(string $message = '') {
    $this->_message = $message;
    parent::__construct('Warning');
  }

  /**
   * @throws PHPUnit_Framework_Exception
   */
  public function runTest(): void {
    throw new WarningException($this->_message);
  }

  /**
   * @return string
   *
   * @since Method available since Release 3.0.0
   */
  public function getMessage(): string {
    return $this->_message;
  }

  /**
   * Returns a string representation of the test case.
   *
   * @return string
   *
   * @since Method available since Release 3.4.0
   */
  public function toString(): string {
    return 'Warning';
  }

}
