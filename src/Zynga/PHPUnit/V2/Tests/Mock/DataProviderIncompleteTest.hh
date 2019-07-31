<?hh // strict

namespace Zynga\PHPUnit\V2\Tests\Mock;

use Zynga\PHPUnit\V2\TestCase;
use Zynga\PHPUnit\V2\Tests\Mock\SampleClass;
use Zynga\PHPUnit\V2\Tests\Mock\Struct;

class DataProviderIncompleteTest extends TestCase {

  // Expected: incomplete as the user space code marks this test as incomplete.
  <<dataProvider("incompleteProvider")>>
  public function testIncomplete(int $a, int $b, int $c): void {
    $this->markTestIncomplete('userspace set status to incomplete');
    $this->assertTrue(true);
  }

  public static function incompleteProvider(): array<array<int>> {
    return [[0, 0, 0]];
  }

  // Expected: incomplete as the return set includes a non compatible amount of params.
  <<dataProvider("badDataSetProvider")>>
  public function testBadDataSet(int $a, int $b, int $c): void {
    $this->assertTrue(true);
  }

  public static function badDataSetProvider(): array<array<int>> {
    return array(array());
  }

  // Expected: incomplete as the return set does not match types with the types for this function.
  <<dataProvider("badValueTypeProvider")>>
  public function testBadValueTypes(int $a, string $b): void {
    $this->assertTrue(true);
  }

  public static function badValueTypeProvider(): array<array<mixed>> {
    return array(array(1, 1));
  }

  // Expected: incomplete as the return set does not match types with the types for this function.
  <<dataProvider("badObjectTypeProvider")>>
  public function testBadObjectTypeProvider(SampleClass $a): void {
    $this->assertTrue(true);
  }

  public static function badObjectTypeProvider(): Struct {
    return new Struct(1.0);
  }

  // Expected: Incomplete as the function that we are calling to is not-static.
  <<dataProvider("nonStaticProvider")>>
  public function testNonStaticProvider(int $a): void {
    $this->assertTrue(true);
  }

  public function nonStaticProvider(): array<array<int>> {
    return array(array(1));
  }

  <<dataProvider("allNativeTypesProvider")>>
  public function testAllNativeTypesProvider(
    int $a,
    string $b,
    string $d,
    float $e,
  ): void {
    $this->assertTrue(true);
  }

  public static function allNativeTypesProvider(): array<array<mixed>> {
    return array(array(1, 'foo', "bar", 2.0));
  }

  // Expected: Incomplete as the function does not exist.
  <<dataProvider("missingFunction")>>
  public function testMissingFunction(int $a): void {
    $this->assertTrue(true);
  }

  // Expected: OK, as this is a valid dataset to work with.
  <<dataProvider("addProvider")>>
  public function testAdd(int $a, int $b, int $c): void {
    $this->assertEquals($c, $a + $b);
  }

  public static function addProvider(): array<array<int>> {
    return [[0, 0, 0], [0, 1, 1]];
  }

  // Expected: OK, as this is a valid object to work with.
  <<dataProvider("objectProvider")>>
  public function testObject(SampleClass $a): void {
    $this->assertTrue(true);
  }

  public static function objectProvider(): SampleClass {
    return new SampleClass(1, 2, 3);
  }

}
