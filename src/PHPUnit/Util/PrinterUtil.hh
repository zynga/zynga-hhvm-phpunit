<?hh // strict

namespace PHPUnit\Util;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use PHPUnit\Exceptions\Exception as PHPUnit_Exceptions_Exception;
use PHPUnit\Util\InvalidArgumentHelperUtil;

/**
 * Utility class that can print to STDOUT or write to a file.
 *
 * @since Class available since Release 2.0.0
 */
abstract class PrinterUtil {
    /**
     * If true, flush output after every write.
     *
     * @var bool
     */
    protected bool $autoFlush = false;

    /**
     * @var resource
     */
    protected resource $out;

    /**
     * @var string
     */
    protected string $outTarget;

    /**
     * @var bool
     */
    protected bool $printsHTML = false;

    /**
     * Constructor.
     *
     * @param mixed $out
     *
     * @throws PHPUnit_Exceptions_Exception
     */
    public function __construct(mixed $out = null) {

      if ($out !== null) {

        if (is_string($out)) {

          if (strpos($out, 'socket://') === 0) {

            $uriPieces = explode(':', str_replace('socket://', '', $out));

            if (count($uriPieces) != 2) {
              throw new PHPUnit_Exceptions_Exception('logging uri failed to parse uri=' . $out);
            }

            $this->out = fsockopen($uriPieces[0], $uriPieces[1]);

          } else {

            if (strpos($out, 'php://') === false && !is_dir(dirname($out))) {
              mkdir(dirname($out), 0777, true);
            }

            $this->out = fopen($out, 'wt');

          }

          $this->outTarget = $out;
        } elseif ( is_resource($out) ) {
          $this->out = $out;
          $this->outTarget = 'custom';
        } else {
          throw new PHPUnit_Exceptions_Exception('invalid output setup');
        }

      } else {
        throw new PHPUnit_Exceptions_Exception('invalid output setup');
      }


    }

    /**
     * Flush buffer, optionally tidy up HTML, and close output if it's not to a php stream
     */
    public function flush(): void {

      if ($this->out && strncmp($this->outTarget, 'php://', 6) !== 0) {
        fclose($this->out);
      }

      // --
      // JEO: We don't support tidy_* usage within the forked phpunit, consdering
      // removal of this code.
      // --
      /*
      if ($this->printsHTML === true &&
          $this->outTarget !== null &&
          strpos($this->outTarget, 'php://') !== 0 &&
          strpos($this->outTarget, 'socket://') !== 0 &&
          extension_loaded('tidy')) {
          file_put_contents(
              $this->outTarget,
              tidy_repair_file(
                  $this->outTarget,
                  ['indent' => true, 'wrap' => 0],
                  'utf8'
              )
          );
      }
      */
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
      if ($this->out) {
        fflush($this->out);
      } else {
        flush();
      }
    }

    public function write(Map<string, mixed> $buffer): void {

      $this->writeString($this->writeHook($buffer));

    }

    abstract public function writeHook(Map<string, mixed> $buffer): string;

    /**
     * @param string $buffer
     */
    public function writeString(string $buffer): void {
        if ($this->out) {
            fwrite($this->out, $buffer);

            if ($this->autoFlush) {
                $this->incrementalFlush();
            }
        } else {

          $sapi = null;

          if ( defined('PHP_SAPI') ) {
            $sapi = constant('PHP_SAPI');
          }

          if ($sapi != 'cli' && $sapi != 'phpdbg') {
            $buffer = htmlspecialchars($buffer);
          }

          print $buffer;

          if ($this->autoFlush) {
            $this->incrementalFlush();
          }

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
    public function setAutoFlush(bool $autoFlush): void
    {
        if (is_bool($autoFlush)) {
            $this->autoFlush = $autoFlush;
        } else {
            throw InvalidArgumentHelperUtil::factory(1, 'boolean');
        }
    }
}
