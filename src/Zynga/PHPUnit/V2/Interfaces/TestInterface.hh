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
   * The current class name, to lower footprint of calling get_class() all the
   * time.
   *
   * @return string
   */
  public function getClass(): string;

  /**
   * Sets the name for the testable item depending on context it's either a
   * test name or suite name.
   *
   * @return bool
   */
  public function setName(string $name): bool;

  /**
   * Fetches the configured name for the testable item.
   *
   * @return string
   */
  public function getName(): string;

  /**
   * Runs a test and collects its result in a TestResult instance.
   *
   * @param TestResult $result
   *
   * @return TestResult
   */
  public function run(?TestResult $result = null): TestResult;

  public function setDependenciesFromAnnotation(): bool;
  public function setDependencies(Vector<string> $deps): bool;

  public function getGroupsFromAnnotation(): Vector<string>;
  public function setGroupsFromAnnotation(): bool;
  public function getGroups(): Vector<string>;

  public function getAnnotations(): Map<string, Map<string, Vector<string>>>;
  public function getAllAnnotationsForKey(string $key): Vector<string>;
  public function getAnnotationsForKey(
    string $context,
    string $key,
  ): Vector<string>;

}
