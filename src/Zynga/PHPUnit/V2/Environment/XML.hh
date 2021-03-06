<?hh // strict

namespace Zynga\PHPUnit\V2\Environment;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use Zynga\PHPUnit\V2\Environment\Strings;
use Zynga\PHPUnit\V2\Exceptions\XML\LoadException;
use Zynga\Framework\Dynamic\V1\DynamicClassCreation;
use Zynga\Framework\ReflectionCache\V1\ReflectionClasses;

use \DOMCharacterData;
use \DOMDocument;
use \DOMElement;
use \DOMNode;
use \DOMText;

/**
 * XML helpers.
 *
 * @since Class available since Release 3.2.0
 */
class XML {
  /**
   * Load an $actual document into a DOMDocument.  This is called
   * from the selector assertions.
   *
   * If $actual is already a DOMDocument, it is returned with
   * no changes.  Otherwise, $actual is loaded into a new DOMDocument
   * as either HTML or XML, depending on the value of $isHtml. If $isHtml is
   * false and $xinclude is true, xinclude is performed on the loaded
   * DOMDocument.
   *
   * Note: prior to PHPUnit 3.3.0, this method loaded a file and
   * not a string as it currently does.  To load a file into a
   * DOMDocument, use loadFile() instead.
   *
   * @param string|DOMDocument $actual
   * @param bool               $isHtml
   * @param string             $filename
   * @param bool               $xinclude
   * @param bool               $strict
   *
   * @return DOMDocument
   *
   * @since Method available since Release 3.3.0
   */
  public static function load(
    mixed $actual,
    bool $isHtml = false,
    string $filename = '',
    bool $xinclude = false,
    bool $strict = false,
  ): DOMDocument {
    if ($actual instanceof DOMDocument) {
      return $actual;
    }

    if (!is_string($actual)) {
      throw new LoadException('Could not load XML from '.gettype($actual));
    }

    if ($actual === '') {
      throw new LoadException('Could not load XML from empty string');
    }

    // Required for XInclude on Windows.
    if ($xinclude) {
      $cwd = getcwd();
      @chdir(dirname($filename));
    }

    $document = new DOMDocument();
    $document->preserveWhiteSpace = false;

    $internal = libxml_use_internal_errors(true);
    $message = '';
    $reporting = error_reporting(0);

    if ('' !== $filename) {
      // Necessary for xinclude
      $document->documentURI = $filename;
    }

    if ($isHtml) {
      $loaded = $document->loadHTML($actual);
    } else {
      $loaded = $document->loadXML($actual);
    }

    if (!$isHtml && $xinclude) {
      $document->xinclude();
    }

    foreach (libxml_get_errors() as $error) {
      $message .= "\n".$error->message;
    }

    libxml_use_internal_errors($internal);
    error_reporting($reporting);

    // if ($xinclude) {
    //   @chdir($cwd);
    // }

    if ($loaded === false || ($strict && $message !== '')) {
      if ($filename !== '') {
        throw new LoadException(
          sprintf(
            'Could not load "%s".%s',
            $filename,
            $message != '' ? "\n".$message : '',
          ),
        );
      } else {
        if ($message === '') {
          $message = 'Could not load XML for unknown reason';
        }
        throw new LoadException($message);
      }
    }

    return $document;
  }

  /**
   * Loads an XML (or HTML) file into a DOMDocument object.
   *
   * @param string $filename
   * @param bool   $isHtml
   * @param bool   $xinclude
   * @param bool   $strict
   *
   * @return DOMDocument
   *
   * @since Method available since Release 3.3.0
   */
  public static function loadFile(
    string $filename,
    bool $isHtml = false,
    bool $xinclude = false,
    bool $strict = false,
  ): DOMDocument {
    $reporting = error_reporting(0);
    $contents = file_get_contents($filename);
    error_reporting($reporting);

    if ($contents === false) {
      throw new LoadException(sprintf('Could not read "%s".', $filename));
    }

    return self::load($contents, $isHtml, $filename, $xinclude, $strict);
  }

  /**
   * @param DOMNode $node
   *
   * @since Method available since Release 3.3.0
   */
  public static function removeCharacterDataNodes(DOMNode $node): void {
    if ($node->hasChildNodes()) {
      for ($i = $node->childNodes->length - 1; $i >= 0; $i--) {
        if (($child = $node->childNodes->item($i)) instanceof DOMCharacterData) {
          $node->removeChild($child);
        }
      }
    }
  }

  /**
   * Escapes a string for the use in XML documents
   * Any Unicode character is allowed, excluding the surrogate blocks, FFFE,
   * and FFFF (not even as character reference).
   * See http://www.w3.org/TR/xml/#charsets
   *
   * @param string $string
   *
   * @return string
   *
   * @since Method available since Release 3.4.6
   */
  public static function prepareString(string $string): string {
    return preg_replace(
      '/[\\x00-\\x08\\x0b\\x0c\\x0e-\\x1f\\x7f]/',
      '',
      htmlspecialchars(Strings::convertToUtf8($string), ENT_QUOTES, 'UTF-8'),
    );
  }

  /**
   * "Convert" a DOMElement object into a PHP variable.
   *
   * @param DOMElement $element
   *
   * @return mixed
   *
   * @since Method available since Release 3.4.0
   */
  public static function xmlToVariable(DOMElement $element): mixed {
    $variable = null;

    switch ($element->tagName) {

      case 'array':
        $variable = [];

        $offset = 0;
        foreach ($element->getElementsByTagName('element') as $element) {
          $item = $element->childNodes->item(0);

          if ($item instanceof DOMText) {
            $item = $element->childNodes->item(1);
          }

          $value = self::xmlToVariable($item);

          if ($element->hasAttribute('key')) {
            $variable[(string) $element->getAttribute('key')] = $value;
          } else {
            $variable[$offset] = $value;
            $offset++;
          }

        }
        break;

      case 'object':
        $className = $element->getAttribute('class');

        if ($element->hasChildNodes()) {
          $arguments = $element->childNodes->item(1)->childNodes;
          $constructorArgs = [];

          foreach ($arguments as $argument) {
            if ($argument instanceof DOMElement) {
              $constructorArgs[] = self::xmlToVariable($argument);
            }
          }

          $class = ReflectionClasses::getReflection($className);
          if ($class !== null) {
            $variable = $class->newInstanceArgs($constructorArgs);
          }
        } else {
          // JEO: Need to change this to utilize dynamicism.
          $params = Vector {};
          $variable =
            DynamicClassCreation::createClassByName($className, $params);
        }
        break;

      case 'boolean':
        $variable = $element->textContent == 'true' ? true : false;
        break;

      case 'integer':
      case 'double':
      case 'string':
        $variable = $element->textContent;

        settype($variable, $element->tagName);
        break;
    }

    return $variable;
  }
}
