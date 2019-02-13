<?hh // strict

namespace Zynga\PHPUnit\V2\Tests\Mock;

use Zynga\PHPUnit\V2\Tests\Mock\ParentClassWithProtectedAttributes;

class ClassWithNonPublicAttributes
  extends ParentClassWithProtectedAttributes {
  public static string $publicStaticAttribute = 'foo';
  protected static string $protectedStaticAttribute = 'bar';
  protected static string $privateStaticAttribute = 'baz';

  public string $publicAttribute = 'foo';
  public int $foo = 1;
  public int $bar = 2;
  protected string $protectedAttribute = 'bar';
  protected string $privateAttribute = 'baz';

  public Vector<string> $publicArray = Vector {'foo'};
  protected Vector<string> $protectedArray = Vector {'bar'};
  protected Vector<string> $privateArray = Vector {'baz'};
}
