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
use SebastianBergmann\CodeCoverage\Driver\Xdebug;
use SebastianBergmann\CodeCoverage\Driver\HHVM\LineStack;
use SebastianBergmann\CodeCoverage\Driver\HHVM\Logging as HHVM_Logging;
use SebastianBergmann\CodeCoverage\ProcessedFile\ProcessedFile;
use SebastianBergmann\CodeCoverage\ProcessedFile\FileContainer;
use SebastianBergmann\TokenStream\Stream\CachingFactory;

use \RuntimeException;

/**
 * Driver for HHVM's code coverage functionality.
 *
 */
class HHVM {

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
    xdebug_start_code_coverage();
  }

  /**
   * Stop collection of code coverage information.
   *
   * @return array
   */
  public function stop(string $testId): void {

    $data = xdebug_get_code_coverage();

    xdebug_stop_code_coverage();

    foreach ($data as $fileName => $execStatuses) {

      $processedFile = FileContainer::get($fileName);

      echo "caputuring fileName=$fileName\n";
      foreach ($execStatuses as $lineNo => $lineState) {
        $processedFile->setLineToTest($lineNo, $testId);
        if ($lineState >= Driver::LINE_EXECUTED) {
          echo "captured $lineNo : executed\n";
          $processedFile->setLineExecutionState(
            $lineNo,
            Driver::LINE_EXECUTED,
          );
        } else {
          $processedFile->setLineExecutionState($lineNo, $lineState);
        }
      }

    }

  }

}
