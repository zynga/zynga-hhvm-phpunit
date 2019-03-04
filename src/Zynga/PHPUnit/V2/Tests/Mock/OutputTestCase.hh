<?hh

namespace Zynga\PHPUnit\V2\Tests\Mock;

use Zynga\PHPUnit\V2\TestCase;

class OutputTestCase extends TestCase {
  public function testExpectOutputStringFooActualFoo() {
    $this->expectOutputString('foo');
    print 'foo';
  }

  public function testExpectOutputStringFooActualBar() {
    $this->expectOutputString('foo');
    print 'bar';
  }

  public function testExpectOutputRegexFooActualFoo() {
    $this->expectOutputRegex('/foo/');
    print 'foo';
  }

  public function testExpectOutputRegexFooActualBar() {
    $this->expectOutputRegex('/foo/');
    print 'bar';
  }
}
