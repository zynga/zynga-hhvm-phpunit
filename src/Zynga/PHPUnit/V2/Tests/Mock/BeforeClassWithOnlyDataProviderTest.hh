<?hh // strict

namespace Zynga\PHPUnit\V2\Tests\Mock;

use Zynga\PHPUnit\V2\TestCase;

<<
beforeClass("someAnnotatedSetupMethod"),
afterClass("someAnnotatedTearDownMethod")
>>
class BeforeClassWithOnlyDataProviderTest extends TestCase {

  public static bool $setUpBeforeClassWasCalled = false;
  public static bool $doSetUpBeforeClassWasCalled = false;
  public static bool $someAnnotatedSetupMethodWasCalled = false;
  public static bool $tearDownAfterClassWasCalled = false;
  public static bool $doTearDownAfterClassWasCalled = false;
  public static bool $someAnnotatedTearDownMethodWasCalled = false;

  public static function resetProperties(): void {
    self::$setUpBeforeClassWasCalled = false;
    self::$doSetUpBeforeClassWasCalled = false;
    self::$someAnnotatedSetupMethodWasCalled = false;
    self::$tearDownAfterClassWasCalled = false;
    self::$doTearDownAfterClassWasCalled = false;
    self::$someAnnotatedTearDownMethodWasCalled = false;
  }

  public function setUpBeforeClass(): void {
    self::$setUpBeforeClassWasCalled = true;
  }

  public function doSetUpBeforeClass(): void {
    self::$doSetUpBeforeClassWasCalled = true;
  }

  public function someAnnotatedSetupMethod(): void {
    self::$someAnnotatedSetupMethodWasCalled = true;
  }

  public function tearDownAfterClass(): void {
    self::$tearDownAfterClassWasCalled = true;
  }

  public function doTearDownAfterClass(): void {
    self::$doTearDownAfterClassWasCalled = true;
  }

  public function someAnnotatedTearDownMethod(): void {
    self::$someAnnotatedTearDownMethodWasCalled = true;
  }

  public static function dummyProvider(): array<array<mixed>> {
    return array(array(1));
  }

  <<dataProvider("dummyProvider")>>
  public function testDummy(mixed $someVal): void {
    $this->assertFalse(false);
  }

}
