<?hh // strict

namespace Zynga\PHPUnit\V2\Filter;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use Zynga\PHPUnit\V2\Interfaces\FilterInterface;
use Zynga\PHPUnit\V2\Filter\Test as TestFilter;

/**
 * @since Class available since Release 4.0.0
 */
class Container {
  private static Vector<FilterInterface> $_excludeGroupFilters = Vector {};
  private static Vector<FilterInterface> $_includeGroupFilters = Vector {};
  private static Vector<FilterInterface> $_testFilters = Vector {};

  public static function addGroupIncludeFilter(string $groups): bool {
    return true;
  }

  public static function addGroupExcludeFilter(string $groups): bool {

    //$testFilter = new TestFilter($testRegex);
    //self::$_testFilters->add($testFilter);

    return true;

  }

  public static function addTestFilter(string $testRegex): bool {

    $testFilter = new TestFilter($testRegex);

    self::$_testFilters->add($testFilter);

    return true;

  }

}
