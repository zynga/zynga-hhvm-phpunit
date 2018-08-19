<?hh // strict

namespace SebastianBergmann\TextTemplate;

/*
 * This file is part of the Text_Template package.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use \InvalidArgumentException;
use \RuntimeException;

/**
 * A simple template engine.
 *
 * @since Class available since Release 1.0.0
 */
class Template {
  /**
   * @var string
   */
  protected string $template = '';

  /**
   * @var string
   */
  protected string $openDelimiter = '{';

  /**
   * @var string
   */
  protected string $closeDelimiter = '}';

  /**
   * Constructor.
   *
   * @param  string                   $file
   * @throws InvalidArgumentException
   */
  public function __construct(
    string $file = '',
    string $openDelimiter = '{',
    string $closeDelimiter = '}',
  ) {
    $this->setFile($file);
    $this->openDelimiter = $openDelimiter;
    $this->closeDelimiter = $closeDelimiter;
  }

  /**
   * Sets the template file.
   *
   * @param  string                   $file
   * @throws InvalidArgumentException
   */
  public function setFile(string $file, string $extension = '.dist'): void {
    $distFile = $file.$extension;

    if (file_exists($file)) {
      $this->template = file_get_contents($file);
    } else if (file_exists($distFile)) {
      $this->template = file_get_contents($distFile);
    } else {
      throw new InvalidArgumentException(
        'Template file could not be loaded.',
      );
    }
  }

  public function setOpenDelimiter(string $delim): void {
    $this->openDelimiter = $delim;
  }

  public function setCloseDelimiter(string $delim): void {
    $this->closeDelimiter = $delim;
  }

  /**
   * Renders the template and returns the result.
   *
   * @return string
   */
  public function render(
    Map<string, mixed> $values,
    bool $trimTemplate = false,
  ): string {

    $searchStrings = array();
    $replaceValues = array();

    foreach ($values as $key => $value) {

      $templateKey = $this->openDelimiter.$key.$this->closeDelimiter;

      $searchStrings[] = $templateKey;
      $replaceValues[] = $value;

    }

    $rendered = str_replace($searchStrings, $replaceValues, $this->template);

    if ($trimTemplate === true) {
      return trim($rendered);
    }

    return $rendered;

  }

  /**
   * Renders the template and writes the result to a file.
   *
   * @param string $target
   */
  public function renderTo(string $target, Map<string, mixed> $values): void {
    $fp = @fopen($target, 'wt');
    if (is_resource($fp)) {
      fwrite($fp, $this->render($values));
      fclose($fp);
    } else {
      $error = error_get_last();

      throw new RuntimeException(
        sprintf(
          'Could not write to %s: %s',
          $target,
          substr($error['message'], strpos($error['message'], ':') + 2),
        ),
      );
    }
  }
}
