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

use SebastianBergmann\CodeCoverage\Driver\Xdebug;
use SebastianBergmann\CodeCoverage\Driver\HHVM\LineStack;
use SebastianBergmann\CodeCoverage\Driver\HHVM\Logging as HHVM_Logging;
use SebastianBergmann\CodeCoverage\ProcessedFile\FileContainer;
    
/**
 * Driver for HHVM's code coverage functionality.
 *
 * @codeCoverageIgnore
 */
class HHVM extends Xdebug {

  public function __construct() {

    parent::__construct();

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
  public function stop(): array<string, array<int, int>> {

    $data = parent::stop();

    // --
    // JEO: We overload the stop function to enable us to fill in the data that
    //   is currently missing within hhvm's xdebug.
    // --
    // --
    // If there is more than one executionary pass, mark it as ran.
    // --

    $returnData = Map {};

    foreach ($data as $file => $rawExecStack) {

      // only process real files.
      if (!file_exists($file)) {
        echo "nonExistant file=$file\n";
        continue;
      }

      // exclude any files that aren't hack related.
      // --
      // JEO: This is due to our inclusion chain within legacy php code, and how
      // we track results within this function.
      // --
      if (!preg_match('/.hh$/', $file)) {
        unset($data[$file]);
        continue;
      }

      HHVM_Logging::debug("file=$file");

      $fileStack = FileContainer::get($file);

      // --
      // HHVM xdebug reports the number of times something is executed,
      //  whereas php:xdebug just does a 0/1 state.
      // --
      // $fileStack->consumeRawExecStack($rawExecStack);

      $returnData->set($file, $fileStack->lineExecutionStatetoArrayFormat());

    }

    return $returnData->toArray();

  }

}
