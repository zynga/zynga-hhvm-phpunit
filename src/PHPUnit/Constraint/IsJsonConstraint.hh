<?hh // strict

namespace PHPUnit\Constraint;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use PHPUnit\Constraint\Base as ConstraintBase;
use PHPUnit\Constraint\JsonMatches\ErrorMessageProvider;
/**
 * Constraint that asserts that a string is valid JSON.
 *
 * @since Class available since Release 3.7.20
 */
class IsJsonConstraint extends ConstraintBase {
    /**
     * Evaluates the constraint for parameter $other. Returns true if the
     * constraint is met, false otherwise.
     *
     * @param mixed $other Value or object to evaluate.
     *
     * @return bool
     */
    public function matches(mixed $other): bool {

      if ($other === '') {
        return false;
      }

      json_decode($other);

      if (json_last_error()) {
        return false;
      }

      return true;

    }

    /**
     * Returns the description of the failure
     *
     * The beginning of failure messages is "Failed asserting that" in most
     * cases. This method should return the second part of that sentence.
     *
     * @param mixed $other Evaluated value or object.
     *
     * @return string
     */
    public function failureDescription(mixed $other): string {

      if ($other === '') {
        return 'an empty string is valid JSON';
      }

      json_decode($other);

      $error = ErrorMessageProvider::determineJsonError(
        json_last_error()
      );

      return sprintf(
        '%s is valid JSON (%s)',
        $this->exporter->shortenedExport($other),
        $error
      );

    }

    /**
     * Returns a string representation of the constraint.
     *
     * @return string
     */
    public function toString(): string {
      return 'is valid JSON';
    }

}