<?hh // strict

namespace PHPUnit\Tests\Mock;

use PHPUnit\Tests\Mock\ParentClassWithPrivateAttributes;

class ParentClassWithProtectedAttributes
  extends ParentClassWithPrivateAttributes {
  protected static string $protectedStaticParentAttribute = 'foo';
  protected string $protectedParentAttribute = 'bar';
}
