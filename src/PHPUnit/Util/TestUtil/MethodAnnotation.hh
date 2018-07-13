<?hh // strict

namespace PHPUnit\Util\TestUtil;

use Zynga\Framework\Annotations\V1\Annotation;

class MethodAnnotation {
  public Map<string, string> $classAnnotations;
  public Map<string, string> $methodAnnotations;

  public function __construct(Vector<Annotation> $classAnnotations, Vector<Annotation> $methodAnnotations) {
    $this->classAnnotations = Map {};
    $this->methodAnnotations = Map {};

    foreach ( $classAnnotations as $classAnnotation ) {
      $this->classAnnotations->set(
        $classAnnotation->name,
        $classAnnotation->value
      );
    }

    foreach ( $methodAnnotations as $methodAnnotation ) {
      $this->methodAnnotations->set(
        $methodAnnotation->name,
        $methodAnnotation->value
      );
    }

  }

}
