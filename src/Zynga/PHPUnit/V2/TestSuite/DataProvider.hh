<?hh // strict

namespace Zynga\PHPUnit\V2\TestSuite;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use Zynga\PHPUnit\V2\TestSuite;

/**
 * @since Class available since Release 3.4.0
 */
class DataProvider extends TestSuite {
  /**
   * Sets the dependencies of a TestCase.
   *
   * @param array $dependencies
   */
  public function setDependencies(Vector<string> $dependencies): bool {
    foreach ($this->tests() as $test) {
      $test->setDependencies($dependencies);
    }
    return true;
  }
}
