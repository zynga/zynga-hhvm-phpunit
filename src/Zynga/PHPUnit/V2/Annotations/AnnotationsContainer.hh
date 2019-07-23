<?hh // strict

namespace Zynga\PHPUnit\V2\Annotations;

class AnnotationsContainer {
  private Map<string, Vector<string>> $_annotations;

  public function __construct() {
    $this->_annotations = Map {};
  }

  public function addValuesFromAttributes(mixed $attributes): bool {
    if (is_array($attributes)) {
      foreach ($attributes as $key => $value) {
        $this->addValueFromAttribute($key, $value);
      }
    }
    return true;
  }

  public function addValueFromAttribute(string $key, mixed $value): bool {
    if (is_string($value)) {
      return $this->addValueToKey($key, $value);
    }
    if (is_array($value)) {
      foreach ($value as $elem) {
        return $this->addValueFromAttribute($key, $elem);
      }
      return true;
    }
    return false;
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
