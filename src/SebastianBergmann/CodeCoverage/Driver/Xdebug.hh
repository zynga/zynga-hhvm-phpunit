<?hh // strict

/*
 * This file is part of the php-code-coverage package.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace SebastianBergmann\CodeCoverage\Driver;

use SebastianBergmann\CodeCoverage\Driver;
use SebastianBergmann\CodeCoverage\RuntimeException;

/**
 * Driver for Xdebug's code coverage functionality.
 *
 * @codeCoverageIgnore
 */
class Xdebug implements Driver {
  /**
   * Cache the number of lines for each file
   *
   * @var array
   */
  private Map<string, int> $cacheNumLines = Map {};

  /**
   * Constructor.
   */
  public function __construct() {
    if (!extension_loaded('xdebug')) {
      throw new RuntimeException('This driver requires Xdebug');
    }

    if (version_compare(phpversion('xdebug'), '2.2.1', '>=') &&
        !ini_get('xdebug.coverage_enable')) {
      throw new RuntimeException(
        'xdebug.coverage_enable=On has to be set in php.ini',
      );
    }
  }

  /**
   * Start collection of code coverage information.
   *
   * @param bool $determineUnusedAndDead
   */
  public function start(bool $determineUnusedAndDead = true): void {
    $unusedFlag = constant('XDEBUG_CC_UNUSED');
    $deadCodeFlag = constant('XDEBUG_CC_DEAD_CODE');

    if ($determineUnusedAndDead === true &&
        $unusedFlag !== null &&
        $deadCodeFlag !== null) {
      xdebug_start_code_coverage($unusedFlag | $deadCodeFlag);
    } else {
      xdebug_start_code_coverage();
    }
  }

  /**
   * Stop collection of code coverage information.
   *
   * @return array
   */
  public function stop(): array<string, array<int, int>> {
    $data = xdebug_get_code_coverage();
    xdebug_stop_code_coverage();
    return $this->cleanup($data);
  }

  /**
   * @param array $data
   *
   * @return array
   */
  public function cleanup(
    array<string, array<int, int>> $data,
  ): array<string, array<int, int>> {

    foreach (array_keys($data) as $file) {
      unset($data[$file][0]);

      if (strpos($file, 'xdebug://debug-eval') !== 0 && file_exists($file)) {
        $numLines = $this->getNumberOfLinesInFile($file);

        foreach (array_keys($data[$file]) as $line) {
          if ($line > $numLines) {
            unset($data[$file][$line]);
          }
        }
      }
    }

    return $data;
  }

  /**
   * @param string $file
   *
   * @return int
   */
  public function getNumberOfLinesInFile(string $file): int {

    $cachedLineCount = $this->cacheNumLines->containsKey($file);

    if (!is_int($cachedLineCount)) {

      $cachedLineCount = 0;

      $buffer = file_get_contents($file);
      $cachedLineCount = substr_count($buffer, "\n");

      if (substr($buffer, -1) !== "\n") {
        $cachedLineCount++;
      }

      $this->cacheNumLines->set($file, $cachedLineCount);
    }

    return $cachedLineCount;

  }
}
