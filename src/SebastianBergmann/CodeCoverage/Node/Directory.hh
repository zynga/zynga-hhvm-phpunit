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
use SebastianBergmann\TokenStream\Token\StreamClassStructure;
use Zynga\CodeBase\V1\Code\Code_Method;
/**
 * Represents a directory in the code coverage information tree.
 */
class Directory extends AbstractNode {

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
  public function getClasses(): Map<string, StreamClassStructure> {
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
  public function getTraits(): Map<string, StreamClassStructure> {
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

    $loc = 0;
    $cloc = 0;
    $ncloc = 0;

    foreach ($this->files as $file) {
      $fileLinesOfCode = $file->getLinesOfCode();

      $f_loc = intval($fileLinesOfCode->get('loc'));
      $f_cloc = intval($fileLinesOfCode->get('cloc'));
      $f_ncloc = intval($fileLinesOfCode->get('ncloc'));

      $loc += $f_loc;

    }

    $linesOfCode = Map {'loc' => $loc, 'cloc' => $cloc, 'ncloc' => $ncloc};

    return $linesOfCode;

  }

  /**
   * Returns the number of executable lines.
   *
   * @return int
   */
  public function getNumExecutableLines(bool $recalculate = false): int {
    $numExecutableLines = 0;
    foreach ($this->files as $file) {
      $numExecutableLines += $file->getNumExecutableLines($recalculate);
    }
    return $numExecutableLines;
  }

  /**
   * Returns the number of executed lines.
   *
   * @return int
   */
  public function getNumExecutedLines(bool $recalculate = false): int {
    $numExecutedLines = 0;
    foreach ($this->files as $file) {
      $numExecutedLines += $file->getNumExecutedLines($recalculate);
    }
    return $numExecutedLines;
  }

  /**
   * Returns the number of classes.
   *
   * @return int
   */
  public function getNumClasses(bool $recalculate = false): int {
    $numClasses = 0;
    foreach ($this->files as $file) {
      $numClasses += $file->getNumClasses($recalculate);
    }
    return $numClasses;
  }

  /**
   * Returns the number of tested classes.
   *
   * @return int
   */
  public function getNumTestedClasses(bool $recalculate = false): int {
    $numTestedClasses = 0;
    foreach ($this->files as $file) {
      $numTestedClasses += $file->getNumTestedClasses($recalculate);
    }
    return $numTestedClasses;
  }

  /**
   * Returns the number of traits.
   *
   * @return int
   */
  public function getNumTraits(bool $recalculate = false): int {
    $numTraits = 0;
    foreach ($this->files as $file) {
      $numTraits += $file->getNumTraits($recalculate);
    }
    return $numTraits;
  }

  /**
   * Returns the number of tested traits.
   *
   * @return int
   */
  public function getNumTestedTraits(bool $recalculate = false): int {
    $numTestedTraits = 0;
    foreach ($this->files as $file) {
      $numTestedTraits += $file->getNumTestedTraits($recalculate);
    }
    return $numTestedTraits;
  }

  /**
   * Returns the number of methods.
   *
   * @return int
   */
  public function getNumMethods(bool $recalculate = false): int {
    $numMethods = 0;
    foreach ($this->files as $file) {
      $numMethods += $file->getNumMethods($recalculate);
    }
    return $numMethods;
  }

  /**
   * Returns the number of tested methods.
   *
   * @return int
   */
  public function getNumTestedMethods(bool $recalculate = false): int {
    $numTestedMethods = 0;
    foreach ($this->files as $file) {
      $numTestedMethods += $file->getNumTestedMethods($recalculate);
    }
    return $numTestedMethods;
  }

  /**
   * Returns the number of functions.
   *
   * @return int
   */
  public function getNumFunctions(bool $recalculate = false): int {
    $numFunctions = 0;
    foreach ($this->files as $file) {
      $numFunctions += $file->getNumFunctions($recalculate);
    }
    return $numFunctions;
  }

  /**
   * Returns the number of tested functions.
   *
   * @return int
   */
  public function getNumTestedFunctions(bool $recalculate = false): int {
    $numTestedFunctions = 0;
    foreach ($this->files as $file) {
      $numTestedFunctions += $file->getNumTestedFunctions($recalculate);
    }
    return $numTestedFunctions;
  }
}
