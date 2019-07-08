<?hh // strict

namespace Zynga\PHPUnit\V2\Annotations;

class AnnotationsContainer {
  private Map<string, Vector<string>> $_annotations;

  public function __construct() {
    $this->_annotations = Map {};
  }

  public function getValuesAsMap(): Map<string, Vector<string>> {
    return $this->_annotations;
  }

  public function setValues(Map<string, Vector<string>> $values): bool {
    $this->_annotations = $values;
    return true;
  }

  public function addValueToKey(string $key, string $value): bool {

    $values = $this->_annotations->get($key);

    if (!$values instanceof Vector) {
      $values = Vector {};
      $this->_annotations->set($key, $values);
    }

    $values->add($value);

    return true;

  }

  public function getValuesForKey(string $key): Vector<string> {

    $values = $this->_annotations->get($key);

    if ($values instanceof Vector) {
      return $values;
    }

    return Vector {};

  }

}
