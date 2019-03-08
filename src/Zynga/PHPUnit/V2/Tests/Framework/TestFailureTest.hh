<?hh // strict

namespace Zynga\PHPUnit\V2\Tests\Framework;

use SebastianBergmann\PHPUnit\Exceptions\Exception;
use Zynga\PHPUnit\V2\TestCase;
use Zynga\PHPUnit\V2\TestFailure;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

/**
 * @since      File available since Release 3.7.20
 */
class TestFailureTest extends TestCase {
  /**
   * @covers TestFailure::toString
   */
  public function testToString(): void {
    $test = new self(__FUNCTION__);
    $exception = new Exception('message');
    $failure = new TestFailure($test, $exception);

    $this->assertEquals(__METHOD__.': message', $failure->toString());
  }

}
