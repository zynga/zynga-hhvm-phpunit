<?hh // strict

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace PHPUnit\Util;

use \RecursiveIterator;

use \PHPUnit_Framework_Test;
use \PHPUnit_Framework_TestSuite;

// --
// JEO: RecursiveIterator would work in this context, except there was a mess
// with the current function returning null upon current calls.
// --
/**
 * Iterator for test suites.
 *
 * @since Class available since Release 3.1.0
 */
// JEO: removed => implements RecursiveIterator<PHPUnit_Framework_Test>
class UtilTestSuiteIterator  {
    /**
     * @var int
     */
    protected int $position;

    /**
     * @var PHPUnit_Framework_Test[]
     */
    protected Vector<PHPUnit_Framework_Test> $tests;

    /**
     * @param PHPUnit_Framework_TestSuite $testSuite
     */
    public function __construct(PHPUnit_Framework_TestSuite $testSuite) {
      $this->position = 0;
      $this->tests = Vector {};

      foreach ( $testSuite->tests() as $test ) {
        $this->tests->add($test);
      }

    }

    /**
     * Rewinds the Iterator to the first element.
     */
    public function rewind(): void {
      $this->position = 0;
    }

    /**
     * Checks if there is a current element after calls to rewind() or next().
     *
     * @return bool
     */
    public function valid(): bool {
      return $this->position < $this->tests->count();
    }

    /**
     * Returns the key of the current element.
     *
     * @return int
     */
    public function key(): int {
        return $this->position;
    }

    /**
     * Returns the current element.
     *
     * @return PHPUnit_Framework_Test
     */
    public function current(): ?PHPUnit_Framework_Test {
      if ( $this->valid() === true ) {
        return $this->tests[$this->position];
      }
      return null;
    }

    /**
     * Moves forward to next element.
     */
    public function next(): void {
      $this->position++;
    }

    /**
     * Returns the sub iterator for the current element.
     *
     * @return UtilTestSuiteIterator
     */
    public function getChildren(): ?UtilTestSuiteIterator {

      $elem = $this->current();

      if ( $elem instanceof PHPUnit_Framework_TestSuite ) {
        return new self($elem);
      }

      return null;

    }

    /**
     * Checks whether the current element has children.
     *
     * @return bool
     */
    public function hasChildren(): bool{
      $elem = $this->current();

      if ( $elem instanceof PHPUnit_Framework_TestSuite ) {
        return true;
      }

      return false;

    }

}
