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
use SebastianBergmann\TokenStream\Token\Stream;

use SebastianBergmann\CodeCoverage\ProcessedFile\FileContainer;
use SebastianBergmann\CodeCoverage\ProcessedFile\ProcessedFile;

use
  SebastianBergmann\TokenStream\Token\Stream\CachingFactory as StreamCachingFactory
;

use SebastianBergmann\TokenStream\Token\StreamMethodStructure;
use SebastianBergmann\TokenStream\Token\StreamTraitStructure;
use SebastianBergmann\TokenStream\Token\StreamClassStructure;

use \Exception;

/**
 * Represents a file in the code coverage information tree.
 */
class File extends AbstractNode {

  /**
   * Constructor.
   *
   * @param string       $name
   * @param AbstractNode $parent
   * @param array        $testData
   *
   * @throws InvalidArgumentException
   */
  public function __construct(string $name, AbstractNode $parent) {

    parent::__construct($name, $parent);

    $this->calculateStatistics();

  }

  public function stream(): Stream {
    $fileName = $this->getPath();
    if (is_string($fileName)) {
      return StreamCachingFactory::get($fileName);
    }
    throw new Exception('stream() called with invalid fileName set');
  }

  public function processedFile(): ProcessedFile {

    $fileName = $this->getPath();

    if (is_string($fileName)) {
      return FileContainer::get($fileName);
    }

    throw new Exception('processedFile() called with invalid fileName set');
  }

  /**
   * Returns the number of files in/under this node.
   *
   * @return int
   */
  public function count(): int {
    return 1;
  }

  /**
   * Returns the code coverage data of this node.
   *
   * @return array
   */
  public function getCoverageData(): array<int, int> {
    throw new Exception(
      'getCoverageData - unsure is this is the right return format',
    );
    $processedFile = $this->processedFile();
    return $processedFile->lineExecutionStatetoArrayFormat();
  }

  /**
   * Returns the classes of this node.
   *
   * @return array
   */
  public function getClasses(): Map<string, StreamClassStructure> {
    $processedFile = $this->processedFile();
    return $processedFile->stream()->getClasses();
  }

  /**
   * Returns the traits of this node.
   *
   * @return array
   */
  public function getTraits(): Map<string, StreamClassStructure> {
    $processedFile = $this->processedFile();
    return $processedFile->stream()->getTraits();
  }

  /**
   * Returns the functions of this node.
   *
   * @return array
   */
  public function getFunctions(): Map<string, StreamMethodStructure> {
    $processedFile = $this->processedFile();
    return $processedFile->stream()->getFunctions();
  }

  /**
   * Returns the LOC/CLOC/NCLOC of this node.
   *
   * @return array
   */
  public function getLinesOfCode(): Map<string, int> {
    $processedFile = $this->processedFile();
    return $processedFile->stream()->getLinesOfCode();
  }

  /**
   * Returns the number of executable lines.
   *
   * @return int
   */
  public function getNumExecutableLines(bool $recalculate = false): int {
    $processedFile = $this->processedFile();
    return $processedFile->getNumExecutedLines($recalculate);
  }

  /**
   * Returns the number of executed lines.
   *
   * @return int
   */
  public function getNumExecutedLines(bool $recalculate = false): int {
    $processedFile = $this->processedFile();
    return $processedFile->getNumExecutedLines($recalculate);
  }

  /**
   * Returns the number of classes.
   *
   * @return int
   */
  public function getNumClasses(bool $recalculate = false): int {
    $processedFile = $this->processedFile();
    return $processedFile->getNumClasses($recalculate);
  }

  /**
   * Returns the number of tested classes.
   *
   * @return int
   */
  public function getNumTestedClasses(bool $recalculate = false): int {
    $processedFile = $this->processedFile();
    return $processedFile->getNumTestedClasses($recalculate);
  }

  /**
   * Returns the number of traits.
   *
   * @return int
   */
  public function getNumTraits(bool $recalculate = false): int {
    $processedFile = $this->processedFile();
    return $processedFile->getNumTraits($recalculate);
  }

  /**
   * Returns the number of tested traits.
   *
   * @return int
   */
  public function getNumTestedTraits(bool $recalculate = false): int {
    $processedFile = $this->processedFile();
    return $processedFile->getNumTestedTraits($recalculate);
  }

  /**
   * Returns the number of methods.
   *
   * @return int
   */
  public function getNumMethods(bool $recalculate = false): int {
    $processedFile = $this->processedFile();
    return $processedFile->getNumMethods($recalculate);
  }

  /**
   * Returns the number of tested methods.
   *
   * @return int
   */
  public function getNumTestedMethods(bool $recalculate = false): int {
    $processedFile = $this->processedFile();
    return $processedFile->getNumTestedMethods($recalculate);
  }

  /**
   * Returns the number of functions.
   *
   * @return int
   */
  public function getNumFunctions(bool $recalculate = false): int {
    $processedFile = $this->processedFile();
    return $processedFile->getNumFunctions($recalculate);
  }

  /**
   * Returns the number of tested functions.
   *
   * @return int
   */
  public function getNumTestedFunctions(bool $recalculate = false): int {
    $processedFile = $this->processedFile();
    return $processedFile->getNumFunctions($recalculate);
  }

  /**
   * Calculates coverage statistics for the file.
   */
  protected function calculateStatistics(): void {
    $this->processClasses();
    $this->processTraits();
    $this->processFunctions();
  }

  /**
   * @param Stream $tokens
   */
  protected function processClasses(): void {
    $classes = $this->getClasses();

    $link = $this->getId().'.html#';

    foreach ($classes as $className => $class) {

      $class->link = $link.$class->startLine;

      foreach ($class->methods as $methodName => $method) {
        $method->link = $link.$method->startLine;
      }

    }

  }

  /**
   * @param Stream $tokens
   */
  protected function processTraits(): void {

    $traits = $this->getTraits();

    $link = $this->getId().'.html#';

    foreach ($traits as $traitName => $trait) {

      $trait->link = $link.$trait->startLine;

      foreach ($trait->methods as $methodName => $method) {
        $method->link = $link.$method->startLine;
      }

    }

  }

  /**
   * @param Stream $tokens
   */
  protected function processFunctions(): void {
    $functions = $this->getFunctions();

    $link = $this->getId().'.html#';

    foreach ($functions as $functionName => $function) {
      $function->link = $link.$function->startLine;
    }

  }

}
