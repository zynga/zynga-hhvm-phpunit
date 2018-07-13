<?hh // strict

namespace PHPUnit\Interfaces;

use \Countable;

use \PHPUnit_Framework_TestResult;
use PHPUnit\Util\TestUtil\MethodAnnotation;

interface TestInterface extends Countable {
  public function addToAssertionCount(int $count): void;
  public function getActualOutput(): string;
  public function getAnnotations(): MethodAnnotation;
  public function getCount(): int;
  public function getGroups(): Vector<string>;
  public function getName(): string;
  public function getNumAssertions(): int;
  public function getResult(): PHPUnit_Framework_TestResult;
  public function getSize(): int;
  public function getTestResultObject(): PHPUnit_Framework_TestResult;
  public function hasExpectationOnOutput(): bool;
  public function hasPerformedExpectationsOnOutput(): bool;
  public function hasOutput(): bool;
  public function isSmall(): bool;
  public function isMedium(): bool;
  public function isLarge(): bool;
  public function resetCount(): bool;
  public function run(?PHPUnit_Framework_TestResult $result = null): ?PHPUnit_Framework_TestResult;
  public function runBare(): void;
  public function setGroups(Vector<string> $groups): bool;
  public function setRegisterMockObjectsFromTestArgumentsRecursively(bool $flag): void;
}
