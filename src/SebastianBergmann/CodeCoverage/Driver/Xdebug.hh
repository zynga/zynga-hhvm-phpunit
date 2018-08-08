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
use SebastianBergmann\TokenStream\Token\Stream\CachingFactory;

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
  public function stop(): Map<string, Map<int, int>> {
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
  ): Map<string, Map<int, int>> {

    $return = Map {};

    foreach ($data as $fileName => $fileData) {

      if (strpos($fileName, 'xdebug://debug-eval') !== 0 &&
          file_exists($fileName)) {
        // do nothing and exclude the file from the stack.
      } else {

        $fileStack = $return->get($fileName);

        if (!$fileStack instanceof Map) {
          $fileStack = Map {};
        }

        $fileLinesOfCode = $this->getNumberOfLinesInFile($fileName);

        foreach ($fileData as $lineNo => $lineState) {

          if ($lineNo > $fileLinesOfCode) {
            continue;
          }

          $fileStack->set($lineNo, $lineState);

        }

      }

    }

    return $return;

  }

  /**
   * @param string $file
   *
   * @return int
   */
  public function getNumberOfLinesInFile(string $file): int {
    $fileStream = CachingFactory::get($file);
    $fileLinesOfCode = $fileStream->getLineCount();
    return $fileLinesOfCode;
  }
}
