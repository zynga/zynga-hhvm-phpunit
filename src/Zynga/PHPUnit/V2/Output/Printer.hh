<?hh // strict

namespace Zynga\PHPUnit\V2\Output;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

/**
 * Utility class that can print to STDOUT or write to a file.
 *
 * @since Class available since Release 2.0.0
 */
class Printer {
  protected bool $autoFlush = false;

  /**
   * Flush buffer, optionally tidy up HTML, and close output if it's not to a php stream
   */
  public function flush(): void {
    flush();
  }

  /**
   * Performs a safe, incremental flush.
   *
   * Do not confuse this function with the flush() function of this class,
   * since the flush() function may close the file being written to, rendering
   * the current object no longer usable.
   *
   * @since Method available since Release 3.3.0
   */
  public function incrementalFlush(): void {
    flush();
  }

  /**
   * @param string $buffer
   */
  public function write(string $buffer): void {

    print $buffer;

    if ($this->autoFlush) {
      $this->incrementalFlush();
    }

  }

  /**
   * Check auto-flush mode.
   *
   * @return bool
   *
   * @since Method available since Release 3.3.0
   */
  public function getAutoFlush(): bool {
    return $this->autoFlush;
  }

  /**
   * Set auto-flushing mode.
   *
   * If set, *incremental* flushes will be done after each write. This should
   * not be confused with the different effects of this class' flush() method.
   *
   * @param bool $autoFlush
   *
   * @since Method available since Release 3.3.0
   */
  public function setAutoFlush(bool $autoFlush): void {
    $this->autoFlush = $autoFlush;

  }
}
