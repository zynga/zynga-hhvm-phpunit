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
class UtilTestSuiteIterator  implements Iterator<PHPUnit_Framework_Test> {

    /**
     * @var PHPUnit_Framework_Test[]
     */
    protected Vector<PHPUnit_Framework_Test> $tests;
    protected KeyedIterator<int, PHPUnit_Framework_Test> $testIterator;

    /**
     * @param PHPUnit_Framework_TestSuite $testSuite
     */
    public function __construct(PHPUnit_Framework_TestSuite $testSuite) {
      $this->tests = Vector {};

      foreach ( $testSuite->tests() as $test ) {
        $this->tests->add($test);
      }

      $this->testIterator = $this->tests->getIterator();

    }

    /**
     * Rewinds the Iterator to the first element.
     */
    public function rewind(): void {
      $this->testIterator->rewind();
    }

    /**
     * Checks if there is a current element after calls to rewind() or next().
     *
     * @return bool
     */
    public function valid(): bool {
      $valid = $this->testIterator->valid();
      return $valid;
    }

    /**
     * Returns the key of the current element.
     *
     * @return int
     */
    public function key(): int {
      $pos = $this->testIterator->key();
      return $pos;
    }

    /**
     * Returns the current element.
     *
     * @return PHPUnit_Framework_Test
     */
    public function current(): PHPUnit_Framework_Test {
      $elem = $this->testIterator->current();
      return $elem;
    }

    /**
     * Moves forward to next element.
     */
    public function next(): void {
      $this->testIterator->next();
    }

    /**
     * Returns the sub iterator for the current element.
     *
     * @return UtilTestSuiteIterator
     */
    /*
    public function getChildren(): this {
      return $this;
      /*
      $children = null;

      $elem = $this->current();

      if ( $elem instanceof PHPUnit_Framework_TestSuite ) {
        $children = new UtilTestSuiteIterator($elem);
      } else {
        $children = new UtilTestSuiteIterator(new PHPUnit_Framework_TestSuite('', ''));
      }

      error_log('JEO getChildren elem=' . get_class($elem) . ' children=' . json_encode($children));
      return $children;
    }
    */

    /**
     * Checks whether the current element has children.
     *
     * @return bool
     */
    /*
    public function hasChildren(): bool {

      $hasChildren = false;

      $elem = $this->current();

      if ( $elem instanceof PHPUnit_Framework_TestSuite ) {
        $hasChildren = true;
      }

      error_log('JEO hasChildren elem=' . get_class($elem) . ' hasChildren=' . json_encode($hasChildren));
      return $hasChildren;

    }
    */

}
