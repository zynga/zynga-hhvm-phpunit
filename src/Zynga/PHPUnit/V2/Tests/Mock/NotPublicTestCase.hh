<?hh // strict

namespace Zynga\PHPUnit\V2\Tests\Mock;

use Zynga\PHPUnit\V2\TestCase;

class NotPublicTestCase extends TestCase {

  // This should show up as testable
  public function testPublic(): void {}

  // This should show as a failure, as it's hidden from view.
  protected function testNotPublicIsProtected(): void {}

  // This should show as a failure, as it's hidden from view.
  private function testNotPublicIsPrivate(): void {}

}
