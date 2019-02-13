<?hh // strict

namespace Zynga\PHPUnit\V2\Tests\Mock;

class ParentClassWithPrivateAttributes {
  private static string $privateStaticParentAttribute = 'foo';
  private string $privateParentAttribute = 'bar';
}
