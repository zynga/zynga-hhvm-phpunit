<?hh

namespace PHPUnit\Tests\Framework;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use \PHPUnit_Framework_TestCase;
use \PHPUnit_Framework_TestResult;
use PHPUnit\Tests\Mock\BaseTestListenerSample;
use PHPUnit\Tests\Mock\Success;

/**
 * @since      Class available since Release 4.0.0
 */
class BaseTestListenerTest extends PHPUnit_Framework_TestCase {

  public function doSetUpBeforeClass(): void {
    // noop
  }

  public function doTearDownAfterClass(): void {
    // noop
  }

  // @TODO: This test is currently emitting a message about closing more than
  // it's own outputbuffers. Please ignore until everything gets pushed into
  // strict.
  public function testEndEventsAreCounted() {
    $result = new PHPUnit_Framework_TestResult();
    $listener = new BaseTestListenerSample();
    $result->addListener($listener);
    $test = new Success();
    $test->run($result);
    $this->assertEquals(1, $listener->endCount);
  }

}
