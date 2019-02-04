<?hh // strict

namespace SebastianBergmann\PHPUnit\Assertions;

use SebastianBergmann\PHPUnit\Assertions;
use SebastianBergmann\PHPUnit\Environment\XML;

use \DOMDocument;
use \DOMElement;

class AssertEqualXMLStructure {

  /**
   * Asserts that a hierarchy of DOMElements matches.
   *
   * @param DOMElement $expectedElement
   * @param DOMElement $actualElement
   * @param bool       $checkAttributes
   * @param string     $message
   *
   * @since Method available since Release 3.3.0
   */
  public static function evaluate(
    Assertions $assertions,
    DOMElement $expectedElement,
    DOMElement $actualElement,
    bool $checkAttributes = false,
    string $message = '',
  ): bool {

    $tmp = new DOMDocument();
    $expectedElement = $tmp->importNode($expectedElement, true);

    $tmp = new DOMDocument();
    $actualElement = $tmp->importNode($actualElement, true);

    $tmp = null;

    $assertions->assertEquals(
      $expectedElement->tagName,
      $actualElement->tagName,
      $message,
    );

    if ($checkAttributes) {
      $assertions->assertEquals(
        $expectedElement->attributes->length,
        $actualElement->attributes->length,
        sprintf(
          '%s%sNumber of attributes on node "%s" does not match',
          $message,
          strlen($message) == 0 ? "\n" : '',
          $expectedElement->tagName,
        ),
      );

      for ($i = 0; $i < $expectedElement->attributes->length; $i++) {
        $expectedAttribute = $expectedElement->attributes->item($i);
        $actualAttribute =
          $actualElement->attributes->getNamedItem($expectedAttribute->name);

        if (!$actualAttribute) {
          $assertions->fail(
            sprintf(
              '%s%sCould not find attribute "%s" on node "%s"',
              $message,
              !strlen($message) == 0 ? "\n" : '',
              $expectedAttribute->name,
              $expectedElement->tagName,
            ),
          );
        }
      }
    }

    XML::removeCharacterDataNodes($expectedElement);
    XML::removeCharacterDataNodes($actualElement);

    $assertions->assertEquals(
      $expectedElement->childNodes->length,
      $actualElement->childNodes->length,
      sprintf(
        '%s%sNumber of child nodes of "%s" differs',
        $message,
        !strlen($message) == 0 ? "\n" : '',
        $expectedElement->tagName,
      ),
    );

    for ($i = 0; $i < $expectedElement->childNodes->length; $i++) {
      self::evaluate(
        $assertions,
        $expectedElement->childNodes->item($i),
        $actualElement->childNodes->item($i),
        $checkAttributes,
        $message,
      );
    }

    return true;

  }

}
