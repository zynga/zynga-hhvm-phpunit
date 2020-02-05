<?hh

/*
 * This file is part of the Diff package.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace SebastianBergmann\Diff\LCS;

use SebastianBergmann\Diff\LCS\LongestCommonSubsequence;
use \SplFixedArray;

/**
 * Time-efficient implementation of longest common subsequence calculation.
 */
class TimeEfficientImplementation implements LongestCommonSubsequence {

  /**
   * Calculates the longest common subsequence of two arrays.
   *
   * @param array $from
   * @param array $to
   *
   * @return array
   */
  public function calculate(array $from, array $to): array {
    $common = array();
    $fromLength = count($from);
    $toLength = count($to);
    $width = $fromLength + 1;

    $matrix = new SplFixedArray($width * ($toLength + 1));

    for ($i = 0; $i <= $fromLength; ++$i) {
      $matrix->offsetSet($i, 0);
    }

    for ($j = 0; $j <= $toLength; ++$j) {
      $matrix->offsetSet($j * $width, 0);
    }

    for ($i = 1; $i <= $fromLength; ++$i) {
      for ($j = 1; $j <= $toLength; ++$j) {
        $o = ($j * $width) + $i;
        $matrix->offsetSet(
          $o,
          max(
            $matrix->offsetGet($o - 1),
            $matrix->offsetGet($o - $width),
            $from[$i - 1] === $to[$j - 1]
              ? $matrix->offsetGet($o - $width - 1) + 1
              : 0,
          ),
        );
      }
    }

    $i = $fromLength;
    $j = $toLength;

    while ($i > 0 && $j > 0) {
      if ($from[$i - 1] === $to[$j - 1]) {
        $common[] = $from[$i - 1];
        --$i;
        --$j;
      } else {
        $o = ($j * $width) + $i;
        if ($matrix->offsetGet($o - $width) > $matrix->offsetGet($o - 1)) {
          --$j;
        } else {
          --$i;
        }
      }
    }

    return array_reverse($common);
  }
}
