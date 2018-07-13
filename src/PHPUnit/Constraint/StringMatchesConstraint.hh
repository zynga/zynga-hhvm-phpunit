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

use PHPUnit\Constraint\PCREMatchConstraint;
use SebastianBergmann\Diff\Differ;

/**
 * ...
 *
 * @since Class available since Release 3.5.0
 */
class StringMatchesConstraint extends PCREMatchConstraint {

    /**
     * @var string
     */
    protected string $string;

    /**
     * @param string $string
     */
    public function __construct(mixed $string) {

      $string = strval($string);

      parent::__construct('');

      $this->string = $string;

    }

    public function matches(mixed $other): bool {

      if ( $this->pattern == '' ) {
        $this->pattern = $this->createPatternFromFormat(
          preg_replace('/\r\n/', "\n", $this->string)
        );
      }

      return parent::matches($other);
    }

    public function failureDescription(mixed $other): string {
      return 'format description matches text';
    }

    public function additionalFailureDescription(mixed $other): string {

      $other = strval($other);

      $from = preg_split('(\r\n|\r|\n)', $this->string);
      $to   = preg_split('(\r\n|\r|\n)', $other);

      foreach ($from as $index => $line) {

        if (is_array($to) && array_key_exists($index, $to) && $line !== $to[$index]) {

          $line = $this->createPatternFromFormat($line);

          if (preg_match($line, $to[$index]) > 0) {
            $from[$index] = $to[$index];
          }

        }

      }

      $this->string = implode("\n", $from);
      $other        = implode("\n", $to);

      $differ = new Differ("--- Expected\n+++ Actual\n");

      return $differ->diff($this->string, $other);

    }

    public function createPatternFromFormat(string $string): string {
      $string = str_replace(
        [
          '%e',
          '%s',
          '%S',
          '%a',
          '%A',
          '%w',
          '%i',
          '%d',
          '%x',
          '%f',
          '%c'
        ],
        [
          '\\' . DIRECTORY_SEPARATOR,
          '[^\r\n]+',
          '[^\r\n]*',
          '.+',
          '.*',
          '\s*',
          '[+-]?\d+',
          '\d+',
          '[0-9a-fA-F]+',
          '[+-]?\.?\d+\.?\d*(?:[Ee][+-]?\d+)?',
          '.'
        ],
        preg_quote($string, '/')
      );

      return '/^' . $string . '$/s';

    }
}
