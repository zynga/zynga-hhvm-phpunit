<?hh // strict

namespace PHPUnit\Constraint;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use PHPUnit\Constraint\Base as ConstraintBase;
use \Countable;
/**
 * @since Class available since Release 3.6.0
 */
class SameSizeConstraint extends ConstraintBase {

    /**
     * @var int
     */
    protected int $expectedCount;

    /**
     * @param int $expected
     */
    public function __construct(mixed $expected) {

      parent::__construct();

      $this->expectedCount = 0;

      if ( is_int($expected) ) {
        $this->expectedCount = $expected;
      } elseif ( $expected instanceof Countable || is_array($expected) ) {
        $this->expectedCount = count($expected);
      }

    }

    /**
     * @return string
     */
    public function toString(): string {
      return sprintf(
        'count matches %d',
        $this->expectedCount
      );
    }
    
}
