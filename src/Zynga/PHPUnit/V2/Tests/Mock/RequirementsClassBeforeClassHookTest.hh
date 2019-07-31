<?hh // strict

namespace Zynga\PHPUnit\V2\Tests\Mock;

use Zynga\PHPUnit\V2\TestCase;

use \Exception;

<<requires("extension nonExistingExtension")>>
class RequirementsClassBeforeClassHookTest extends TestCase {
  public function setUpBeforeClass(): void {
    throw new Exception(
      __METHOD__.' should not be called because of class requirements.',
    );
  }
}
