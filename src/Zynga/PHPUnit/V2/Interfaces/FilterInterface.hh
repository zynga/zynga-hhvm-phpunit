<?hh // strict

namespace Zynga\PHPUnit\V2\Interfaces;

interface FilterInterface {
  public function apply(TestInterface $testItem): bool;
}
