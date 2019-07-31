<?hh // strict

namespace Zynga\PHPUnit\V2\Tests\Mock;

use Zynga\PHPUnit\V2\TestCase;

class OneTestCase extends TestCase {

  public function notAnTestCase(): void {}

  public function testCase(string $arg = ''): void {}

}
