<?hh // strict

/*
 * This file is part of the php-code-coverage package.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace SebastianBergmann\CodeCoverage\Node;

use SebastianBergmann\CodeCoverage\Util;
use Zynga\CodeBase\V1\Code\Code_Class;
use Zynga\CodeBase\V1\Code\Code_Method;

/**
 * Base class for nodes in the code coverage information tree.
 */
abstract class AbstractNode implements \Countable {
  /**
   * @var string
   */
  private string $name;

  /**
   * @var string
   */
  private ?string $path;

  /**
   * @var array
   */
  private ?array<AbstractNode> $pathArray;

  /**
   * @var AbstractNode
   */
  private ?AbstractNode $parent;

  /**
   * @var string
   */
  private ?string $id;

  private int $_numClassesAndTraits;
  private int $_numTestedClassesAndTraits;

  private float $_testedClassesPercent;
  private float $_testedTraitsPercent;
  private float $_testedClassesAndTraitsPercent;
  private float $_testedMethodsPercent;

  /**
   * Constructor.
   *
   * @param string       $name
   * @param AbstractNode $parent
   */
  public function __construct(string $name, ?AbstractNode $parent = null) {

    if (substr($name, -1) == '/') {
      $name = substr($name, 0, -1);
    }

    $this->name = $name;
    $this->parent = $parent;

    $this->pathArray = null;
    $this->path = null;
    $this->id = null;

    $this->_numClassesAndTraits = -1;
    $this->_numTestedClassesAndTraits = -1;

    $this->_testedClassesPercent = -1.0;
    $this->_testedTraitsPercent = -1.0;
    $this->_testedClassesAndTraitsPercent = -1.0;
    $this->_testedMethodsPercent = -1.0;

  }

  /**
   * @return string
   */
  public function getName(): string {
    return $this->name;
  }

  /**
   * @return string
   */
  public function getId(): string {

    if (is_string($this->id)) {
      return $this->id;
    }

    $dirComponents = explode(DIRECTORY_SEPARATOR, $this->name);
    $thisDir = strval(array_pop($dirComponents));

    if (strlen($thisDir) > 0) {
      $this->id = $thisDir;
      return $this->id;
    }

    echo "returning empty for getid=".$this->name."\n";
    return '';

  }

  /**
   * @return string
   */
  public function getPath(): ?string {
    if ($this->path === null) {

      if ($this->parent === null ||
          ($this->parent instanceof AbstractNode &&
           $this->parent->getPath() === null)) {
        $this->path = $this->name;
      } else {
        $this->path = $this->parent->getPath().'/'.$this->name;
      }
    }

    return $this->path;
  }

  /**
   * @return array
   */
  public function getPathAsArray(): array<AbstractNode> {
    if ($this->pathArray === null) {

      if ($this->parent === null) {
        $this->pathArray = [];
      } else {
        $this->pathArray = $this->parent->getPathAsArray();
      }

      $this->pathArray[] = $this;

    }

    return $this->pathArray;
  }

  /**
   * @return AbstractNode
   */
  public function getParent(): ?AbstractNode {
    return $this->parent;
  }

  /**
   * Returns the percentage of classes that has been tested.
   *
   * @param bool $asString
   *
   * @return int|string
   */
  public function getTestedClassesPercent(bool $recalculate = false): mixed {

    if ($this->_testedClassesPercent != -1.0 && $recalculate == false) {
      return $this->_testedClassesPercent;
    }

    $this->_testedClassesPercent = Util::percent(
      $this->getNumTestedClasses($recalculate),
      $this->getNumClasses($recalculate),
    );

    return $this->_testedClassesPercent;

  }

  /**
   * Returns the percentage of traits that has been tested.
   *
   * @param bool $asString
   *
   * @return int|string
   */
  public function getTestedTraitsPercent(bool $recalculate = false): float {

    if ($this->_testedTraitsPercent != -1.0 && $recalculate == false) {
      return $this->_testedTraitsPercent;
    }

    $this->_testedTraitsPercent = Util::percent(
      $this->getNumTestedTraits($recalculate),
      $this->getNumTraits($recalculate),
    );

    return $this->_testedTraitsPercent;

  }

  /**
   * Returns the percentage of traits that has been tested.
   *
   * @param bool $asString
   *
   * @return int|string
   */
  public function getTestedClassesAndTraitsPercent(
    bool $recalculate = false,
  ): float {

    if ($this->_testedClassesAndTraitsPercent != -1.0 &&
        $recalculate == false) {
      return $this->_testedClassesAndTraitsPercent;
    }

    $this->_testedClassesAndTraitsPercent = Util::percent(
      $this->getNumTestedClassesAndTraits($recalculate),
      $this->getNumClassesAndTraits($recalculate),
    );

    return $this->_testedClassesAndTraitsPercent;

  }

  public function getTestedClassesAndTraitsPercentAsString(
    bool $recalculate = false,
  ): string {
    return Util::percentAsString(
      $this->getNumTestedClassesAndTraits($recalculate),
      $this->getNumClassesAndTraits($recalculate),
    );
  }

  /**
   * Returns the percentage of methods that has been tested.
   *
   * @param bool $asString
   *
   * @return int|string
   */
  public function getTestedMethodsPercent(bool $recalculate = false): float {

    if ($this->_testedMethodsPercent != -1.0 && $recalculate == false) {
      return $this->_testedMethodsPercent;
    }

    $this->_testedMethodsPercent = Util::percent(
      $this->getNumTestedMethods($recalculate),
      $this->getNumMethods($recalculate),
    );

    return $this->_testedMethodsPercent;

  }

  public function getTestedMethodsPercentAsString(
    bool $recalculate = false,
  ): string {
    return Util::percentAsString(
      $this->getNumTestedMethods($recalculate),
      $this->getNumMethods($recalculate),
    );
  }

  /**
   * Returns the percentage of executed lines.
   *
   * @param bool $asString
   *
   * @return int
   */
  public function getLineExecutedPercent(bool $recalculate = false): float {
    return Util::percent(
      $this->getNumExecutedLines($recalculate),
      $this->getNumExecutableLines($recalculate),
    );
  }

  public function getLineExecutedPercentAsString(
    bool $recalculate = false,
  ): string {
    return Util::percentAsString(
      $this->getNumExecutedLines($recalculate),
      $this->getNumExecutableLines($recalculate),
    );
  }

  /**
   * Returns the number of classes and traits.
   *
   * @return int
   */
  public function getNumClassesAndTraits(bool $recalculate = false): int {
    if ($this->_numClassesAndTraits != -1 && $recalculate == false) {
      return $this->_numClassesAndTraits;
    }
    $this->_numClassesAndTraits = 0;
    $this->_numClassesAndTraits =
      $this->getNumClasses($recalculate) + $this->getNumTraits($recalculate);
    return $this->_numClassesAndTraits;
  }

  /**
   * Returns the number of tested classes and traits.
   *
   * @return int
   */
  public function getNumTestedClassesAndTraits(
    bool $recalculate = false,
  ): int {
    if ($this->_numTestedClassesAndTraits != -1 && $recalculate == false) {
      return $this->_numTestedClassesAndTraits;
    }

    $this->_numTestedClassesAndTraits = 0;
    $this->_numTestedClassesAndTraits =
      $this->getNumTestedClasses($recalculate) +
      $this->getNumTestedTraits($recalculate);

    return $this->_numTestedClassesAndTraits;

  }

  /**
   * Returns the classes and traits of this node.
   *
   * @return array
   */
  public function getClassesAndTraits(): Map<string, Code_Class> {
    $combined = Map {};
    foreach ($this->getClasses() as $className => $classObj) {
      $combined->set($className, $classObj);
    }
    foreach ($this->getTraits() as $className => $classObj) {
      $combined->set($className, $classObj);
    }
    return $combined;
  }

  /**
   * Returns the classes of this node.
   *
   * @return array
   */
  abstract public function getClasses(): Map<string, Code_Class>;

  /**
   * Returns the traits of this node.
   *
   * @return array
   */
  abstract public function getTraits(): Map<string, Code_Class>;

  /**
   * Returns the functions of this node.
   *
   * @return array
   */
  abstract public function getFunctions(): Map<string, Code_Method>;

  /**
   * Returns the LOC/CLOC/NCLOC of this node.
   *
   * @return array
   */
  abstract public function getLinesOfCode(): Map<string, int>;

  /**
   * Returns the number of executable lines.
   *
   * @return int
   */
  abstract public function getNumExecutableLines(
    bool $recalculate = false,
  ): int;

  /**
   * Returns the number of executed lines.
   *
   * @return int
   */
  abstract public function getNumExecutedLines(
    bool $recalculate = false,
  ): int;

  /**
   * Returns the number of classes.
   *
   * @return int
   */
  abstract public function getNumClasses(bool $recalculate = false): int;

  /**
   * Returns the number of tested classes.
   *
   * @return int
   */
  abstract public function getNumTestedClasses(
    bool $recalculate = false,
  ): int;

  /**
   * Returns the number of traits.
   *
   * @return int
   */
  abstract public function getNumTraits(bool $recalculate = false): int;

  /**
   * Returns the number of tested traits.
   *
   * @return int
   */
  abstract public function getNumTestedTraits(bool $recalculate = false): int;

  /**
   * Returns the number of methods.
   *
   * @return int
   */
  abstract public function getNumMethods(bool $recalculate = false): int;

  /**
   * Returns the number of tested methods.
   *
   * @return int
   */
  abstract public function getNumTestedMethods(
    bool $recalculate = false,
  ): int;

  /**
   * Returns the number of functions.
   *
   * @return int
   */
  abstract public function getNumFunctions(bool $recalculate = false): int;

  /**
   * Returns the number of tested functions.
   *
   * @return int
   */
  abstract public function getNumTestedFunctions(
    bool $recalculate = false,
  ): int;

  public function getCcnAsString(): string {
    return '<abbr title="Cyclomatic Complexity Number (CCN)">CCN</abbr>';
  }

  public function getCrapAsString(): string {
    return '<abbr title="Change Risk Anti-Patterns (CRAP) Index">CRAP</abbr>';
  }

}
