<?hh // strict

namespace Zynga\PHPUnit\V2\Interfaces;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use \Countable;
use Zynga\PHPUnit\V2\TestResult;

/**
 * A Test can be run and collect its results.
 *
 * @since      Interface available since Release 2.0.0
 */
interface TestInterface extends Countable {

  /**
   * Runs a test and collects its result in a TestResult instance.
   *
   * @param TestResult $result
   *
   * @return TestResult
   */
  public function run(?TestResult $result = null): TestResult;

  public function getClass(): string;
  public function getName(): string;

}
