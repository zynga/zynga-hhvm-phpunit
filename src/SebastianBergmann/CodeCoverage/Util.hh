<?hh // strict
/*
 * This file is part of the php-code-coverage package.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace SebastianBergmann\CodeCoverage;

/**
 * Utility methods.
 */
class Util {

  public static function percentAsString(
    int $a,
    int $b,
    bool $fixedWidth = false,
  ): string {

    if ($b == 0) {
      return '';
    }

    $percent = self::percent($a, $b);

    if ($fixedWidth) {
      return sprintf('%6.2F%%', floatval($percent));
    }

    return sprintf('%01.2F%%', floatval($percent));

  }
  /**
   * @param float $a
   * @param float $b
   * @param bool  $asString
   * @param bool  $fixedWidth
   *
   * @return float|int|string
   */
  public static function percent(
    int $a,
    int $b,
    bool $asString = false,
    bool $fixedWidth = false,
  ): float {

    if ($b > 0) {
      $percent = ($a / $b) * 100;
    } else {
      $percent = 100.0;
    }

    return floatval($percent);

  }

}
