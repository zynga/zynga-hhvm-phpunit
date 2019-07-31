<?hh // strict

namespace Zynga\PHPUnit\V2\Tests\Mock;

use Zynga\PHPUnit\V2\TestCase;

class BeforeAndAfterTest extends TestCase {
  public static int $beforeWasRun = 0;
  public static int $afterWasRun = 0;

  public static function resetProperties(): void {
    self::$beforeWasRun = 0;
    self::$afterWasRun = 0;
  }

  <<before()>>
  public function initialSetup(): void {
    self::$beforeWasRun++;
  }

  <<after()>>
  public function finalTeardown(): void {
    self::$afterWasRun++;
  }

  public function test1(): void {
    $this->assertTrue(true);
  }

  public function test2(): void {
    $this->assertTrue(true);
  }

}
