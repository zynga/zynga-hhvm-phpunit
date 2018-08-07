<?hh // strict

/*
 * This file is part of the Environment package.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace SebastianBergmann\Environment;

/**
 * Utility class for HHVM/PHP environment handling.
 */
class Runtime {
  /**
   * @var string
   */
  private static string $binary = '';

  /**
   * Returns true when Xdebug is supported or
   * the runtime used is PHPDBG (PHP >= 7.0).
   *
   * @return bool
   */
  public function canCollectCodeCoverage(): bool {
    return $this->hasXdebug();
  }

  /**
   * Returns the path to the binary of the current runtime.
   *
   * @return string
   */
  public function getBinary(): string {
    // HHVM
    self::$binary = PHP_BINARY;
    return self::$binary;
  }

  /**
   * @return string
   */
  public function getNameWithVersion(): string {
    return $this->getName().' '.$this->getVersion();
  }

  /**
   * @return string
   */
  public function getName(): string {
    return 'HHVM';
  }

  /**
   * @return string
   */
  public function getVendorUrl(): string {
    return 'http://hhvm.com/';
  }

  /**
   * @return string
   */
  public function getVersion(): string {
    return HHVM_VERSION;
  }

  /**
   * Returns true when the runtime used is PHP and Xdebug is loaded.
   *
   * @return bool
   */
  public function hasXdebug(): bool {
    return extension_loaded('xdebug');
  }

  /**
   * Returns true when the runtime used is HHVM.
   *
   * @return bool
   */
  public function isHHVM(): bool {
    return defined('HHVM_VERSION');
  }

}
