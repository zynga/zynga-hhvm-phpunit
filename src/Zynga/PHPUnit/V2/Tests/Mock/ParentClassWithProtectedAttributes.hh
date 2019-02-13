<?hh // strict

namespace Zynga\PHPUnit\V2\Tests\Mock;

use Zynga\PHPUnit\V2\Tests\Mock\ParentClassWithPrivateAttributes;

class ParentClassWithProtectedAttributes
  extends ParentClassWithPrivateAttributes {
  protected static string $protectedStaticParentAttribute = 'foo';
  protected string $protectedParentAttribute = 'bar';
}
