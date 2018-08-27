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

use SebastianBergmann\CodeCoverage\InvalidArgumentException;
use Zynga\CodeBase\V1\Code\Code_Class;
use Zynga\CodeBase\V1\Code\Code_Method;
/**
 * Represents a directory in the code coverage information tree.
 */
class Directory extends AbstractNode {

  private int $_loc = -1;
  private int $_cloc = -1;
  private int $_ncloc = -1;

  private int $_numExecutableLines = -1;
  private int $_numExecutedLines = -1;
  private int $_numClasses = -1;
  private int $_numTestedClasses = -1;
  private int $_numTraits = -1;
  private int $_numTestedTraits = -1;
  private int $_numMethods = -1;
  private int $_numTestedMethods = -1;
  private int $_numFunctions = -1;
  private int $_numTestedFunctions = -1;

  /**
   * @var Directory[]
   */
  private Vector<Directory> $directories = Vector {};

  /**
   * @var File[]
   */
  private Vector<File> $files = Vector {};

  /**
   * Returns the number of files in/under this node.
   *
   * @return int
   */
  public function count(): int {
    return $this->files->count();
  }

  /**
   * Adds a new directory.
   *
   * @param string $name
   *
   * @return Directory
   */
  public function addDirectory(string $name): void {
    $directory = new Directory($name, $this);
    $this->directories->add($directory);
  }

  public function addAlreadyCreatedDirectory(Directory $dir): void {
    $this->directories->add($dir);
  }

  /**
   * Adds a new file.
   *
   * @param string $name
   * @param array  $coverageData
   * @param array  $testData
   * @param bool   $cacheTokens
   *
   * @return File
   *
   * @throws InvalidArgumentException
   */
  public function addFile(string $name): void {

    $fileName = $this->getPath().'/'.$name;

    if (!is_file($fileName)) {
      return;
    }

    $file = null;

    $file = new File($name, $this);
    $this->files->add($file);

  }

  public function addFileFullPath(string $filePath): void {

    $shortPath = str_replace(dirname($filePath), '', $filePath);
    $shortPath = preg_replace('/^\//', '', $shortPath);

    $this->addFile($shortPath);

  }

  /**
   * Returns the directories in this directory.
   *
   * @return array
   */
  public function getDirectories(): Vector<Directory> {
    return $this->directories;
  }

  /**
   * Returns the files in this directory.
   *
   * @return array
   */
  public function getFiles(): Vector<File> {
    return $this->files;
  }

  /**
   * Returns the classes of this node.
   *
   * @return array
   */
  public function getClasses(): Map<string, Code_Class> {
    $allClasses = Map {};

    foreach ($this->files as $file) {

      $traits = $file->getClasses();

      foreach ($traits as $className => $classObj) {
        $allClasses->set($className, $classObj);
      }

    }
    return $allClasses;
  }

  /**
   * Returns the traits of this node.
   *
   * @return array
   */
  public function getTraits(): Map<string, Code_Class> {
    $allTraits = Map {};

    foreach ($this->files as $file) {

      $traits = $file->getTraits();

      foreach ($traits as $traitName => $traitObj) {
        $allTraits->set($traitName, $traitObj);
      }

    }
    return $allTraits;
  }

  /**
   * Returns the functions of this node.
   *
   * @return array
   */
  public function getFunctions(): Map<string, Code_Method> {

    $allFunctions = Map {};

    foreach ($this->files as $file) {

      $functions = $file->getFunctions();

      foreach ($functions as $functionName => $functionObj) {
        $allFunctions->set($functionName, $functionObj);
      }

    }

    return $allFunctions;

  }

  /**
   * Returns the LOC/CLOC/NCLOC of this node.
   *
   * @return array
   */
  public function getLinesOfCode(): Map<string, int> {

    if ($this->_loc != -1) {
      $linesOfCode = Map {
        'loc' => $this->_loc,
        'cloc' => $this->_cloc,
        'ncloc' => $this->_ncloc,
      };
      return $linesOfCode;
    }

    $this->_loc = 0;
    $this->_cloc = 0;
    $this->_ncloc = 0;

    foreach ($this->files as $file) {
      $fileLinesOfCode = $file->getLinesOfCode();

      $f_loc = intval($fileLinesOfCode->get('loc'));
      $f_cloc = intval($fileLinesOfCode->get('cloc'));
      $f_ncloc = intval($fileLinesOfCode->get('ncloc'));

      $this->_loc += $f_loc;
      $this->_cloc += $f_cloc;
      $this->_ncloc += $f_ncloc;

    }

    foreach ($this->getDirectories() as $childDirectory) {

      $dirLinesOfCode = $childDirectory->getLinesOfCode();

      $d_loc = intval($dirLinesOfCode->get('loc'));
      $d_cloc = intval($dirLinesOfCode->get('cloc'));
      $d_ncloc = intval($dirLinesOfCode->get('ncloc'));

      $this->_loc += $d_loc;
      $this->_cloc += $d_cloc;
      $this->_ncloc += $d_ncloc;

    }

    $linesOfCode = Map {
      'loc' => $this->_loc,
      'cloc' => $this->_cloc,
      'ncloc' => $this->_ncloc,
    };
    return $linesOfCode;

  }

  /**
   * Returns the number of executable lines.
   *
   * @return int
   */
  public function getNumExecutableLines(bool $recalculate = false): int {

    if ($this->_numExecutableLines != -1) {
      return $this->_numExecutableLines;
    }

    $this->_numExecutableLines = 0;

    foreach ($this->files as $file) {
      $this->_numExecutableLines +=
        $file->getNumExecutableLines($recalculate);
    }

    foreach ($this->getDirectories() as $childDirectory) {
      $this->_numExecutableLines +=
        $childDirectory->getNumExecutableLines($recalculate);
    }

    return $this->_numExecutableLines;
  }

  /**
   * Returns the number of executed lines.
   *
   * @return int
   */
  public function getNumExecutedLines(bool $recalculate = false): int {

    if ($this->_numExecutedLines != -1 && $recalculate === false) {
      return $this->_numExecutedLines;
    }

    $this->_numExecutedLines = 0;

    foreach ($this->files as $file) {
      $this->_numExecutedLines += $file->getNumExecutedLines($recalculate);
    }

    foreach ($this->getDirectories() as $childDirectory) {
      $this->_numExecutedLines +=
        $childDirectory->getNumExecutedLines($recalculate);
    }

    return $this->_numExecutedLines;

  }

  /**
   * Returns the number of classes.
   *
   * @return int
   */
  public function getNumClasses(bool $recalculate = false): int {

    if ($this->_numClasses != -1 && $recalculate === false) {
      return $this->_numClasses;
    }

    $this->_numClasses = 0;

    foreach ($this->files as $file) {
      $this->_numClasses += $file->getNumClasses($recalculate);
    }

    foreach ($this->getDirectories() as $childDirectory) {
      $this->_numClasses += $childDirectory->getNumClasses($recalculate);
    }

    return $this->_numClasses;

  }

  /**
   * Returns the number of tested classes.
   *
   * @return int
   */
  public function getNumTestedClasses(bool $recalculate = false): int {

    if ($this->_numTestedClasses != -1 && $recalculate === false) {
      return $this->_numTestedClasses;
    }

    $this->_numTestedClasses = 0;

    foreach ($this->files as $file) {
      $this->_numTestedClasses += $file->getNumTestedClasses($recalculate);
    }

    foreach ($this->getDirectories() as $childDirectory) {
      $this->_numTestedClasses +=
        $childDirectory->getNumTestedClasses($recalculate);
    }

    return $this->_numTestedClasses;
  }

  /**
   * Returns the number of traits.
   *
   * @return int
   */
  public function getNumTraits(bool $recalculate = false): int {

    if ($this->_numTraits != -1 && $recalculate === false) {
      return $this->_numTraits;
    }

    $this->_numTraits = 0;

    foreach ($this->files as $file) {
      $this->_numTraits += $file->getNumTraits($recalculate);
    }

    foreach ($this->getDirectories() as $childDirectory) {
      $this->_numTraits += $childDirectory->getNumTraits($recalculate);
    }

    return $this->_numTraits;
  }

  /**
   * Returns the number of tested traits.
   *
   * @return int
   */
  public function getNumTestedTraits(bool $recalculate = false): int {

    if ($this->_numTestedTraits != -1 && $recalculate === false) {
      return $this->_numTestedTraits;
    }

    $this->_numTestedTraits = 0;
    foreach ($this->files as $file) {
      $this->_numTestedTraits += $file->getNumTestedTraits($recalculate);
    }

    foreach ($this->getDirectories() as $childDirectory) {
      $this->_numTestedTraits +=
        $childDirectory->getNumTestedTraits($recalculate);
    }

    return $this->_numTestedTraits;
  }

  /**
   * Returns the number of methods.
   *
   * @return int
   */
  public function getNumMethods(bool $recalculate = false): int {

    if ($this->_numMethods != -1 && $recalculate === false) {
      return $this->_numMethods;
    }

    $this->_numMethods = 0;

    foreach ($this->files as $file) {
      $this->_numMethods += $file->getNumMethods($recalculate);
    }

    foreach ($this->getDirectories() as $childDirectory) {
      $this->_numMethods += $childDirectory->getNumMethods($recalculate);
    }

    return $this->_numMethods;
  }

  /**
   * Returns the number of tested methods.
   *
   * @return int
   */
  public function getNumTestedMethods(bool $recalculate = false): int {

    if ($this->_numTestedMethods != -1 && $recalculate === false) {
      return $this->_numTestedMethods;
    }

    $this->_numTestedMethods = 0;

    foreach ($this->files as $file) {
      $this->_numTestedMethods += $file->getNumTestedMethods($recalculate);
    }

    foreach ($this->getDirectories() as $childDirectory) {
      $this->_numTestedMethods +=
        $childDirectory->getNumTestedMethods($recalculate);
    }

    return $this->_numTestedMethods;
  }

  /**
   * Returns the number of functions.
   *
   * @return int
   */
  public function getNumFunctions(bool $recalculate = false): int {

    if ($this->_numFunctions != -1 && $recalculate === false) {
      return $this->_numFunctions;
    }

    $this->_numFunctions = 0;

    foreach ($this->files as $file) {
      $this->_numFunctions += $file->getNumFunctions($recalculate);
    }

    foreach ($this->getDirectories() as $childDirectory) {
      $this->_numFunctions += $childDirectory->getNumFunctions($recalculate);
    }

    return $this->_numFunctions;
  }

  /**
   * Returns the number of tested functions.
   *
   * @return int
   */
  public function getNumTestedFunctions(bool $recalculate = false): int {

    if ($this->_numTestedFunctions != -1 && $recalculate === false) {
      return $this->_numTestedFunctions;
    }

    $this->_numTestedFunctions = 0;

    foreach ($this->files as $file) {
      $this->_numTestedFunctions +=
        $file->getNumTestedFunctions($recalculate);
    }

    foreach ($this->getDirectories() as $childDirectory) {
      $this->_numTestedFunctions +=
        $childDirectory->getNumTestedFunctions($recalculate);
    }

    return $this->_numTestedFunctions;

  }
}
