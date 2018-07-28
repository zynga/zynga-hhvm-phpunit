<?hh // strict

namespace PHPUnit\Tests\Mock;

class ParentClassWithPrivateAttributes {
  private static string $privateStaticParentAttribute = 'foo';
  private string $privateParentAttribute = 'bar';
}
