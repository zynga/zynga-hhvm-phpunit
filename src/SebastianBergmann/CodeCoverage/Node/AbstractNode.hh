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
use SebastianBergmann\TokenStream\Token\StreamClassStructure;
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
  public function getTestedClassesPercent(
    bool $asString = true,
    bool $recalculate = false,
  ): mixed {
    return Util::percent(
      $this->getNumTestedClasses($recalculate),
      $this->getNumClasses($recalculate),
      $asString,
    );
  }

  /**
   * Returns the percentage of traits that has been tested.
   *
   * @param bool $asString
   *
   * @return int|string
   */
  public function getTestedTraitsPercent(
    bool $asString = true,
    bool $recalculate = false,
  ): mixed {
    return Util::percent(
      $this->getNumTestedTraits($recalculate),
      $this->getNumTraits($recalculate),
      $asString,
    );
  }

  /**
   * Returns the percentage of traits that has been tested.
   *
   * @param bool $asString
   *
   * @return int|string
   */
  public function getTestedClassesAndTraitsPercent(
    bool $asString = true,
    bool $recalculate = false,
  ): mixed {
    return Util::percent(
      $this->getNumTestedClassesAndTraits($recalculate),
      $this->getNumClassesAndTraits($recalculate),
      $asString,
    );
  }

  public function getRecursiveTestedClassesAndTraitsPercent(
    bool $asString = true,
    bool $recalculate = false,
  ): mixed {
    return Util::percent(
      $this->getRecursiveNumTestedClassesAndTraits($recalculate),
      $this->getRecursiveNumClassesAndTraits($recalculate),
      $asString,
    );
  }

  /**
   * Returns the percentage of methods that has been tested.
   *
   * @param bool $asString
   *
   * @return int|string
   */
  public function getTestedMethodsPercent(
    bool $asString = true,
    bool $recalculate = false,
  ): mixed {
    return Util::percent(
      $this->getNumTestedMethods($recalculate),
      $this->getNumMethods($recalculate),
      $asString,
    );
  }

  public function getRecursiveTestedMethodsPercent(
    bool $asString = true,
    bool $recalculate = false,
  ): mixed {
    return Util::percent(
      $this->getRecursiveNumTestedMethods($recalculate),
      $this->getRecursiveNumMethods($recalculate),
      $asString,
    );
  }

  /**
   * Returns the percentage of executed lines.
   *
   * @param bool $asString
   *
   * @return int
   */
  public function getLineExecutedPercent(
    bool $asString = true,
    bool $recalculate = false,
  ): mixed {
    return Util::percent(
      $this->getNumExecutedLines($recalculate),
      $this->getNumExecutableLines($recalculate),
      $asString,
    );
  }

  public function getRecursiveLineExecutedPercent(
    bool $asString = true,
    bool $recalculate = false,
  ): mixed {
    return Util::percent(
      $this->getRecursiveNumExecutedLines($recalculate),
      $this->getRecursiveNumExecutableLines($recalculate),
      $asString,
    );
  }

  /**
   * Returns the number of classes and traits.
   *
   * @return int
   */
  public function getNumClassesAndTraits(bool $recalculate = false): int {
    return
      $this->getNumClasses($recalculate) + $this->getNumTraits($recalculate);
  }

  public function getRecursiveNumClassesAndTraits(
    bool $recalculate = false,
  ): int {
    $total = 0;

    $total += $this->getNumClasses($recalculate);
    $total += $this->getNumTraits(false);

    if ($this instanceof Directory) {
      foreach ($this->getDirectories() as $childDirectory) {
        $total +=
          $childDirectory->getRecursiveNumClassesAndTraits($recalculate);
      }
    }

    // echo " grandTotal=$total\n";
    return $total;

  }

  /**
   * Returns the number of tested classes and traits.
   *
   * @return int
   */
  public function getNumTestedClassesAndTraits(
    bool $recalculate = false,
  ): int {
    return
      $this->getNumTestedClasses($recalculate) +
      $this->getNumTestedTraits($recalculate);
  }

  public function getRecursiveNumTestedClassesAndTraits(
    bool $recalculate = false,
  ): int {
    $total = 0;

    $total += $this->getNumTestedClassesAndTraits($recalculate);

    if ($this instanceof Directory) {

      foreach ($this->getDirectories() as $childDirectory) {
        $total += $childDirectory->getRecursiveNumTestedClassesAndTraits(
          $recalculate,
        );
      }

    }

    // echo " grandTotal=$total\n";
    return $total;
  }

  /**
   * Returns the classes and traits of this node.
   *
   * @return array
   */
  public function getClassesAndTraits(): Map<string, StreamClassStructure> {
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
  abstract public function getClasses(): Map<string, StreamClassStructure>;

  /**
   * Returns the traits of this node.
   *
   * @return array
   */
  abstract public function getTraits(): Map<string, StreamClassStructure>;

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

  public function getRecursiveNumExecutableLines(
    bool $recalculate = false,
  ): int {
    $total = 0;

    $total += $this->getNumExecutableLines($recalculate);

    if ($this instanceof Directory) {
      foreach ($this->getDirectories() as $childDirectory) {
        $total += $childDirectory->getNumExecutableLines($recalculate);
      }
    }

    return $total;
  }

  /**
   * Returns the number of executed lines.
   *
   * @return int
   */
  abstract public function getNumExecutedLines(
    bool $recalculate = false,
  ): int;
  public function getRecursiveNumExecutedLines(
    bool $recalculate = false,
  ): int {
    $total = 0;

    $total += $this->getNumExecutedLines($recalculate);

    if ($this instanceof Directory) {
      foreach ($this->getDirectories() as $childDirectory) {
        $total += $childDirectory->getNumExecutedLines($recalculate);
      }
    }

    return $total;
  }

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

  public function getRecursiveNumMethods(bool $recalculate = false): int {
    $total = 0;

    $total += $this->getNumMethods($recalculate);

    if ($this instanceof Directory) {
      foreach ($this->getDirectories() as $childDirectory) {
        $total += $childDirectory->getNumMethods($recalculate);
      }
    }

    return $total;
  }

  /**
   * Returns the number of tested methods.
   *
   * @return int
   */
  abstract public function getNumTestedMethods(
    bool $recalculate = false,
  ): int;
  public function getRecursiveNumTestedMethods(
    bool $recalculate = false,
  ): int {
    $total = 0;

    $total += $this->getNumTestedMethods($recalculate);

    if ($this instanceof Directory) {
      foreach ($this->getDirectories() as $childDirectory) {
        $total += $childDirectory->getNumTestedMethods($recalculate);
      }
    }

    return $total;
  }

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

}
