<?hh // strict

namespace Zynga\PHPUnit\V2\Tests\Mock;

use Zynga\PHPUnit\V2\TestCase;

class BeforeClassWithOnlyDataProviderTest extends TestCase {
  public static bool $setUpBeforeClassWasCalled = false;
  public static bool $beforeClassWasCalled = false;

  public static function resetProperties(): void {
    self::$setUpBeforeClassWasCalled = false;
    self::$beforeClassWasCalled = false;
  }

  public function setUpBeforeClass(): void {
    self::$setUpBeforeClassWasCalled = true;
  }

  <<beforeClass()>>
  public static function someAnnotatedSetupMethod(): void {
    self::$beforeClassWasCalled = true;
  }

  // non-static provider.
  public function dummyProvider(): array<array<mixed>> {
    return [[1]];
  }

  <<dataProvider("dummyProvider")>>
  public function testDummy(mixed $someVal): void {
    $this->assertFalse(false);
  }

}
