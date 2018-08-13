<?hh // strict

namespace Zynga\CodeBase\V1\File;

use Zynga\CodeBase\V1\File;
use Zynga\CodeBase\V1\Code\Code_Inclusion;

class Inclusions {
  private File $_parent;
  private Code_Inclusion $_require;
  private Code_Inclusion $_require_once;
  private Code_Inclusion $_include;
  private Code_Inclusion $_include_once;

  public function __construct(File $parent) {
    $this->_parent = $parent;
    $this->_require = new Code_Inclusion();
    $this->_require_once = new Code_Inclusion();
    $this->_include = new Code_Inclusion();
    $this->_include_once = new Code_Inclusion();
  }

  public function requires(): Code_Inclusion {
    return $this->_require;
  }

  public function require_onces(): Code_Inclusion {
    return $this->_require_once;
  }

  public function includes(): Code_Inclusion {
    return $this->_include;
  }

  public function include_onces(): Code_Inclusion {
    return $this->_include_once;
  }

  public function getAll(): Vector<string> {
    $allInclusions = Vector {};

    $allRequires = $this->requires()->getAll();
    $allInclusions->addAll($allRequires);

    $allRequireOnces = $this->require_onces()->getAll();
    $allInclusions->addAll($allRequireOnces);

    $allIncludes = $this->includes()->getAll();
    $allInclusions->addAll($allIncludes);

    $allIncludeOnces = $this->include_onces()->getAll();
    $allInclusions->addAll($allIncludeOnces);

    return $allInclusions;

  }

  public function getAllAsMap(): Map<string, Vector<string>> {
    $allInclusions = Map {};

    $allRequires = $this->requires()->getAll();
    $allInclusions->set('require', $allRequires);

    $allRequireOnces = $this->require_onces()->getAll();
    $allInclusions->set('require_once', $allRequireOnces);

    $allIncludes = $this->includes()->getAll();
    $allInclusions->set('include', $allIncludes);

    $allIncludeOnces = $this->include_onces()->getAll();
    $allInclusions->set('include_once', $allIncludeOnces);

    return $allInclusions;

  }

  public function getSpecificType(string $type): Vector<string> {

    $allTypes = $this->getAllAsMap();
    $specificType = $allTypes->get($type);

    if ($specificType instanceof Vector) {
      return $specificType;
    }

    $return = Vector {};
    return $return;

  }

}
