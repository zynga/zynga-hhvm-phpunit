<?hh // strict

namespace Zynga\PHPUnit\V2\Constraints;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use SebastianBergmann\Diff\Differ;
use Zynga\PHPUnit\V2\Constraints\PCREMatchConstraint;

/**
 * ...
 *
 * @since Class available since Release 3.5.0
 */
class StringMatchesConstraint extends PCREMatchConstraint {
  private string $string = '';

  public function setExpected(mixed $value): bool {

    if (is_string($value) != true) {
      return false;
    }

    $this->string = strval($value);

    return parent::setExpected(
      $this->createPatternFromFormat(preg_replace('/\r\n/', "\n", $value)),
    );

  }

  public function resetExpected(): bool {

    $this->string = '';

    return parent::resetExpected();

  }

  public function failureDescription(mixed $other): string {
    return 'format description matches text';
  }

  public function additionalFailureDescription(mixed $other): string {

    $from = preg_split('(\r\n|\r|\n)', $this->string);
    $to = preg_split('(\r\n|\r|\n)', $other);

    foreach ($from as $index => $line) {
      if (array_key_exists($index, $to) && $line !== $to[$index]) {
        $line = $this->createPatternFromFormat($line);

        if (preg_match($line, $to[$index]) > 0) {
          $from[$index] = $to[$index];
        }
      }
    }

    $this->string = implode("\n", $from);
    $other = implode("\n", $to);

    $differ = new Differ("--- Expected\n+++ Actual\n");

    return $differ->diff($this->string, $other);

  }

  protected function createPatternFromFormat(string $string): string {
    $string = str_replace(
      ['%e', '%s', '%S', '%a', '%A', '%w', '%i', '%d', '%x', '%f', '%c'],
      [
        '\\'.DIRECTORY_SEPARATOR,
        '[^\r\n]+',
        '[^\r\n]*',
        '.+',
        '.*',
        '\s*',
        '[+-]?\d+',
        '\d+',
        '[0-9a-fA-F]+',
        '[+-]?\.?\d+\.?\d*(?:[Ee][+-]?\d+)?',
        '.',
      ],
      preg_quote($string, '/'),
    );

    return '/^'.$string.'$/s';
  }

}
