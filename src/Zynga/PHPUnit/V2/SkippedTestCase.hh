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

use Zynga\PHPUnit\V2\TestCase;

/**
 * A skipped test case
 *
 * @since Class available since Release 4.3.0
 */
class SkippedTestCase extends TestCase {

  private string $_message = '';

  /**
   * @param string $className
   * @param string $methodName
   * @param string $message
   */
  public function __construct(
    string $className,
    string $methodName,
    string $message = '',
  ) {
    $this->_message = $message;
    parent::__construct($className.'::'.$methodName);
  }

  /**
   * @throws PHPUnit_Framework_Exception
   */
  public function runTest(): void {
    $this->markTestSkipped($this->_message);
  }

  /**
   * @return string
   */
  public function getMessage(): string {
    return $this->_message;
  }

  /**
   * Returns a string representation of the test case.
   *
   * @return string
   */
  public function toString(): string {
    return $this->getName();
  }

}
