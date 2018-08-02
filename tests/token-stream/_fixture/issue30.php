<?hh // strict

class Foo {
  public function bar(): string {
    return Foo::class;
  }
}
