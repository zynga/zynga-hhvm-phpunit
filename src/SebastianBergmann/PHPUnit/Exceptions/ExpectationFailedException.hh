<?hh // strict

namespace SebastianBergmann\PHPUnit\Exceptions;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use SebastianBergmann\PHPUnit\Exceptions\AssertionFailedException;
use SebastianBergmann\Comparator\ComparisonFailure;
use \Exception;

/**
 * Exception for expectations which failed their check.
 *
 * The exception contains the error message and optionally a
 * SebastianBergmann\Comparator\ComparisonFailure which is used to
 * generate diff output of the failed expectations.
 *
 * @since Class available since Release 3.0.0
 */
class ExpectationFailedException extends AssertionFailedException {
  /**
   * @var SebastianBergmann\Comparator\ComparisonFailure
   */
  protected ?ComparisonFailure $comparisonFailure;

  public function __construct(
    string $message,
    ?ComparisonFailure $comparisonFailure = null,
    ?Exception $previous = null,
  ) {

    $this->comparisonFailure = $comparisonFailure;

    parent::__construct($message, 0, $previous);

  }

  /**
   * @return SebastianBergmann\Comparator\ComparisonFailure
   */
  public function getComparisonFailure(): ?ComparisonFailure {
    return $this->comparisonFailure;
  }
}
