<?hh // strict

namespace Zynga\PHPUnit\V2;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use Zynga\PHPUnit\V2\Interfaces\TestInterface;
use \Exception;
use \RecursiveIterator;

/**
 * Iterator for test suites.
 *
 * @since Class available since Release 3.1.0
 */
class TestSuiteIterator {
  private int $_position;
  private Vector<TestInterface> $_tests;

  /**
   * @param TestSuite $testSuite
   */
  public function __construct(TestInterface $testSuite) {

    $this->_position = 0;
    if ($testSuite instanceof TestSuite) {
      $this->_tests = $testSuite->tests();
    } else {
      $this->_tests = Vector {};
    }

  }

  /**
   * Rewinds the Iterator to the first element.
   */
  public function rewind(): void {
    $this->_position = 0;
  }

  /**
   * Checks if there is a current element after calls to rewind() or next().
   *
   * @return bool
   */
  public function valid(): bool {
    return $this->_position < $this->_tests->count();
  }

  /**
   * Returns the key of the current element.
   *
   * @return int
   */
  public function key(): int {
    return $this->_position;
  }

  /**
   * Returns the current element.
   *
   * @return TestInterface
   */
  public function current(): TestInterface {
    $value = $this->_tests->get($this->_position);
    if ($value instanceof TestInterface) {
      return $value;
    }
    throw new Exception('current() is invalid _position='.$this->_position);
  }

  /**
   * Moves forward to next element.
   */
  public function next(): void {
    $this->_position++;
  }

  /**
   * Returns the sub iterator for the current element.
   *
   * @return TestSuiteIterator
   */
  public function getChildren(): Vector<TestInterface> {

    $children = Vector {};

    foreach ($this->_tests as $test) {

      if ($test instanceof TestSuite) {
        $this->_recurseChildren($test, $children);
        continue;
      }

      if ($test instanceof TestInterface) {
        $children->add($test);
      }

    }

    return $children;

  }

  private function _recurseChildren(
    TestSuite $testSuite,
    Vector<TestInterface> $children,
  ): void {

    $grandChildren = $testSuite->tests();

    foreach ($grandChildren as $grandChild) {
      if ($grandChild instanceof TestSuite) {
        $this->_recurseChildren($grandChild, $children);
      }
      if ($grandChild instanceof TestInterface) {
        $children->add($grandChild);
      }
    }

  }

  /**
   * Checks whether the current element has children.
   *
   * @return bool
   */
  public function hasChildren(): bool {
    return $this->_tests[$this->_position] instanceof TestSuite;
  }

}
