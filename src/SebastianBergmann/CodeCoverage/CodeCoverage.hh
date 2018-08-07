<?hh

/*
 * This file is part of the php-code-coverage package.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace SebastianBergmann\CodeCoverage;

use SebastianBergmann\CodeCoverage\Driver;
use SebastianBergmann\CodeCoverage\Driver\Xdebug;
use SebastianBergmann\CodeCoverage\Driver\HHVM;
use SebastianBergmann\CodeCoverage\Driver\PHPDBG;
use SebastianBergmann\CodeCoverage\Node\Builder;
use SebastianBergmann\CodeCoverage\Node\Directory;
use SebastianBergmann\CodeUnitReverseLookup\Wizard;
use SebastianBergmann\Environment\Runtime;

use Zynga\Source\Cache as Zynga_Source_Cache;
use Zynga\Framework\Testing\TestCase\V2\Base as ZyngaTestCaseBase;

use
  SebastianBergmann\TokenStream\Token\Stream\CachingFactory as StreamCachingFactory
;
use SebastianBergmann\CodeCoverage\ProcessedFile\FileContainer;

/**
 * Provides collection functionality for PHP code coverage information.
 */
class CodeCoverage {
  /**
   * @var Driver
   */
  private $driver;

  /**
   * @var Filter
   */
  private $filter;

  /**
   * @var Wizard
   */
  private $wizard;

  /**
   * @var bool
   */
  private $cacheTokens = false;

  /**
   * @var bool
   */
  private $checkForUnintentionallyCoveredCode = false;

  /**
   * @var bool
   */
  private $forceCoversAnnotation = false;

  /**
   * @var bool
   */
  private $checkForUnexecutedCoveredCode = false;

  /**
   * @var bool
   */
  private $checkForMissingCoversAnnotation = false;

  /**
   * @var bool
   */
  private $addUncoveredFilesFromWhitelist = true;

  /**
   * @var bool
   */
  private $processUncoveredFilesFromWhitelist = false;

  /**
   * @var bool
   */
  private $ignoreDeprecatedCode = false;

  /**
   * @var mixed
   */
  private $currentId;

  /**
   * @var array
   */
  private $ignoredLines = [];

  /**
   * @var bool
   */
  private $disableIgnoredLines = false;

  /**
   * Test data.
   *
   * @var array
   */
  private $tests = [];

  /**
   * @var string[]
   */
  private $unintentionallyCoveredSubclassesWhitelist = [];

  /**
   * Determine if the data has been initialized or not
   *
   * @var bool
   */
  private $isInitialized = false;

  /**
   * Determine whether we need to check for dead and unused code on each test
   *
   * @var bool
   */
  private $shouldCheckForDeadAndUnused = true;

  private string $target;

  /**
   * Constructor.
   *
   * @param Driver $driver
   * @param Filter $filter
   *
   * @throws RuntimeException
   */
  public function __construct(
    string $target,
    ?Driver $driver = null,
    ?Filter $filter = null,
  ) {

    $this->target = $target;

    if ($driver === null) {
      $driver = $this->selectDriver();
    }

    if ($filter === null) {
      $filter = new Filter();
    }

    $this->driver = $driver;
    $this->filter = $filter;

    $this->wizard = new Wizard();
  }

  // The destination target directory for output.
  public function getTarget(): string {
    return $this->target;
  }

  /**
   * Returns the code coverage information as a graph of node objects.
   *
   * @return Directory
   */
  public function getReport(): Directory {
    $builder = new Builder();
    return $builder->build($this);
  }

  /**
   * Clears collected code coverage data.
   */
  public function clear() {
    $this->isInitialized = false;
    $this->currentId = null;
    FileContainer::clear();
    $this->tests = [];
  }

  /**
   * Returns the filter object used.
   *
   * @return Filter
   */
  public function filter() {
    return $this->filter;
  }

  /**
   * Returns the collected code coverage data.
   * Set $raw = true to bypass all filters.
   *
   * @param bool $raw
   *
   * @return array
   */
  public function getData($raw = false) {
    if (!$raw && $this->addUncoveredFilesFromWhitelist) {
      $this->addUncoveredFilesFromWhitelist();
    }
    return FileContainer::getAllLineToTestData();
  }

  /**
   * Sets the coverage data.
   *
   * @param array $data
   */
  public function setData(array $data) {
    error_log('JEO - Disabled as the filecontainer handles this now');
    // $this->data = $data;
  }

  /**
   * Returns the test data.
   *
   * @return array
   */
  public function getTests() {
    return $this->tests;
  }

  /**
   * Sets the test data.
   *
   * @param array $tests
   */
  public function setTests(array $tests) {
    $this->tests = $tests;
  }

  /**
   * Start collection of code coverage information.
   *
   * @param mixed $id
   * @param bool  $clear
   *
   * @throws InvalidArgumentException
   */
  public function start($id, $clear = false) {
    if (!is_bool($clear)) {
      throw InvalidArgumentException::create(1, 'boolean');
    }

    if ($clear) {
      $this->clear();
    }

    if ($this->isInitialized === false) {
      $this->initializeData();
    }

    $this->currentId = $id;

    $this->driver->start($this->shouldCheckForDeadAndUnused);
  }

  /**
   * Stop collection of code coverage information.
   *
   * @param bool  $append
   * @param mixed $linesToBeCovered
   * @param array $linesToBeUsed
   *
   * @return array
   *
   * @throws InvalidArgumentException
   */
  public function stop(
    $append = true,
    $linesToBeCovered = [],
    array $linesToBeUsed = [],
  ) {
    if (!is_bool($append)) {
      throw InvalidArgumentException::create(1, 'boolean');
    }

    if (!is_array($linesToBeCovered) && $linesToBeCovered !== false) {
      throw InvalidArgumentException::create(2, 'array or false');
    }

    $data = $this->driver->stop();

    $this->append($data, null, $append, $linesToBeCovered, $linesToBeUsed);

    $this->currentId = null;

    return $data;
  }

  /**
   * Appends code coverage data.
   *
   * @param array $data
   * @param mixed $id
   * @param bool  $append
   * @param mixed $linesToBeCovered
   * @param array $linesToBeUsed
   *
   * @throws RuntimeException
   */
  public function append(
    array $data,
    $id = null,
    $append = true,
    $linesToBeCovered = [],
    array $linesToBeUsed = [],
  ) {
    // @TODO - JEO we are getting rid of this passing around the $data stack shit.
    // echo "appendIsBeingDeprecated!\n";
    return;
    if ($id === null) {
      $id = $this->currentId;
    }

    if ($id === null) {
      throw new RuntimeException();
    }

    $this->applyListsFilter($data);
    //$this->applyIgnoredLinesFilter($data);
    //$this->initializeFilesThatAreSeenTheFirstTime($data);

    if (!$append) {
      return;
    }

    if ($id != 'UNCOVERED_FILES_FROM_WHITELIST') {
      $this->applyCoversAnnotationFilter(
        $data,
        $linesToBeCovered,
        $linesToBeUsed,
      );
    }

    if (empty($data)) {
      return;
    }

    $size = 'unknown';
    $status = null;

    if ($id instanceof ZyngaTestCaseBase ||
        $id instanceof \PHPUnit_Framework_TestCase) {
      $_size = $id->getSize();

      if ($_size == \PHPUnit_Util_Test::SMALL) {
        $size = 'small';
      } else if ($_size == \PHPUnit_Util_Test::MEDIUM) {
        $size = 'medium';
      } else if ($_size == \PHPUnit_Util_Test::LARGE) {
        $size = 'large';
      }

      $status = $id->getStatus();
      $id = get_class($id).'::'.$id->getName();
    } else if ($id instanceof \PHPUnit_Extensions_PhptTestCase) {
      $size = 'large';
      $id = $id->getName();
    }

    $this->tests[$id] = ['size' => $size, 'status' => $status];

    foreach ($data as $file => $lines) {

      if (!$this->filter->isFile($file)) {
        continue;
      }

      $fileStack = FileContainer::get($file);

      foreach ($lines as $lineNo => $lineState) {
        if ($lineState == Driver::LINE_EXECUTED) {
          echo "lineNo=$lineNo id=$id\n";
          $fileStack->setLineToTest($lineNo, $id);
        }
      }

    }
  }

  /**
   * Merges the data from another instance.
   *
   * @param CodeCoverage $that
   */
  public function merge(CodeCoverage $that) {

    error_log('JEO: MERGE_DISABLED, as we made file data singleton');
    return;
    // $this->filter->setWhitelistedFiles(
    //   array_merge(
    //     $this->filter->getWhitelistedFiles(),
    //     $that->filter()->getWhitelistedFiles(),
    //   ),
    // );
    //
    // foreach ($that->data as $file => $lines) {
    //   if (!isset($this->data[$file])) {
    //     if (!$this->filter->isFiltered($file)) {
    //       $this->data[$file] = $lines;
    //     }
    //
    //     continue;
    //   }
    //
    //   foreach ($lines as $line => $data) {
    //     if ($data !== null) {
    //       if (!isset($this->data[$file][$line])) {
    //         $this->data[$file][$line] = $data;
    //       } else {
    //         $this->data[$file][$line] =
    //           array_unique(array_merge($this->data[$file][$line], $data));
    //       }
    //     }
    //   }
    // }
    //
    // $this->tests = array_merge($this->tests, $that->getTests());
  }

  /**
   * @param bool $flag
   *
   * @throws InvalidArgumentException
   */
  public function setCacheTokens($flag) {
    if (!is_bool($flag)) {
      throw InvalidArgumentException::create(1, 'boolean');
    }

    $this->cacheTokens = $flag;
  }

  /**
   * @return bool
   */
  public function getCacheTokens() {
    return $this->cacheTokens;
  }

  /**
   * @param bool $flag
   *
   * @throws InvalidArgumentException
   */
  public function setCheckForUnintentionallyCoveredCode($flag) {
    if (!is_bool($flag)) {
      throw InvalidArgumentException::create(1, 'boolean');
    }

    $this->checkForUnintentionallyCoveredCode = $flag;
  }

  /**
   * @param bool $flag
   *
   * @throws InvalidArgumentException
   */
  public function setForceCoversAnnotation($flag) {
    if (!is_bool($flag)) {
      throw InvalidArgumentException::create(1, 'boolean');
    }

    $this->forceCoversAnnotation = $flag;
  }

  /**
   * @param bool $flag
   *
   * @throws InvalidArgumentException
   */
  public function setCheckForMissingCoversAnnotation($flag) {
    if (!is_bool($flag)) {
      throw InvalidArgumentException::create(1, 'boolean');
    }

    $this->checkForMissingCoversAnnotation = $flag;
  }

  /**
   * @param bool $flag
   *
   * @throws InvalidArgumentException
   */
  public function setCheckForUnexecutedCoveredCode($flag) {
    if (!is_bool($flag)) {
      throw InvalidArgumentException::create(1, 'boolean');
    }

    $this->checkForUnexecutedCoveredCode = $flag;
  }

  /**
   * @deprecated
   *
   * @param bool $flag
   *
   * @throws InvalidArgumentException
   */
  public function setMapTestClassNameToCoveredClassName($flag) {}

  /**
   * @param bool $flag
   *
   * @throws InvalidArgumentException
   */
  public function setAddUncoveredFilesFromWhitelist($flag) {
    if (!is_bool($flag)) {
      throw InvalidArgumentException::create(1, 'boolean');
    }

    $this->addUncoveredFilesFromWhitelist = $flag;
  }

  /**
   * @param bool $flag
   *
   * @throws InvalidArgumentException
   */
  public function setProcessUncoveredFilesFromWhitelist($flag) {
    if (!is_bool($flag)) {
      throw InvalidArgumentException::create(1, 'boolean');
    }

    $this->processUncoveredFilesFromWhitelist = $flag;
  }

  /**
   * @param bool $flag
   *
   * @throws InvalidArgumentException
   */
  public function setDisableIgnoredLines($flag) {
    if (!is_bool($flag)) {
      throw InvalidArgumentException::create(1, 'boolean');
    }

    $this->disableIgnoredLines = $flag;
  }

  /**
   * @param bool $flag
   *
   * @throws InvalidArgumentException
   */
  public function setIgnoreDeprecatedCode($flag) {
    if (!is_bool($flag)) {
      throw InvalidArgumentException::create(1, 'boolean');
    }

    $this->ignoreDeprecatedCode = $flag;
  }

  /**
   * @param array $whitelist
   */
  public function setUnintentionallyCoveredSubclassesWhitelist(
    array $whitelist,
  ) {
    $this->unintentionallyCoveredSubclassesWhitelist = $whitelist;
  }

  /**
   * Applies the @covers annotation filtering.
   *
   * @param array $data
   * @param mixed $linesToBeCovered
   * @param array $linesToBeUsed
   *
   * @throws MissingCoversAnnotationException
   * @throws UnintentionallyCoveredCodeException
   */
  private function applyCoversAnnotationFilter(
    array $data,
    $linesToBeCovered,
    array $linesToBeUsed,
  ) {
    if ($linesToBeCovered === false ||
        ($this->forceCoversAnnotation && empty($linesToBeCovered))) {
      if ($this->checkForMissingCoversAnnotation) {
        throw new MissingCoversAnnotationException();
      }

      $data = [];

      return;
    }

    if (empty($linesToBeCovered)) {

      return;
    }

    if ($this->checkForUnintentionallyCoveredCode) {
      $this->performUnintentionallyCoveredCodeCheck(
        $data,
        $linesToBeCovered,
        $linesToBeUsed,
      );
    }

    if ($this->checkForUnexecutedCoveredCode) {
      $this->performUnexecutedCoveredCodeCheck(
        $data,
        $linesToBeCovered,
        $linesToBeUsed,
      );
    }

    $data = array_intersect_key($data, $linesToBeCovered);

    foreach (array_keys($data) as $filename) {
      $_linesToBeCovered = array_flip($linesToBeCovered[$filename]);

      $data[$filename] =
        array_intersect_key($data[$filename], $_linesToBeCovered);
    }
  }

  /**
   * Applies the whitelist filtering.
   *
   * @param array $data
   */
  private function applyListsFilter(array $data) {
    foreach (array_keys($data) as $filename) {
      if ($this->filter->isFiltered($filename)) {
        unset($data[$filename]);
      }
    }
  }

  /**
   * Applies the "ignored lines" filtering.
   *
   * @param array $data
   */
  private function applyIgnoredLinesFilter(array $data) {
    foreach (array_keys($data) as $filename) {
      if (!$this->filter->isFile($filename)) {
        continue;
      }

      foreach ($this->getLinesToBeIgnored($filename) as $line) {
        unset($data[$filename][$line]);
      }
    }
  }

  /**
   * @param array $data
   */
  private function initializeFilesThatAreSeenTheFirstTime(array $data) {
    // Negated by using the file container approach.
  }

  /**
   * Processes whitelisted files that are not covered.
   */
  private function addUncoveredFilesFromWhitelist() {
    return;

    $filesToThisPoint = FileContainer::getFileNames();

    $uncoveredFiles =
      array_diff($this->filter->getWhitelist(), $filesToThisPoint->toArray());

    foreach ($uncoveredFiles as $uncoveredFile) {

      echo "possibleUncovered=".$uncoveredFile."\n";
      if (!file_exists($uncoveredFile)) {
        continue;
      }

      if (!$this->processUncoveredFilesFromWhitelist) {
        echo
          "processingUncoveredFilesFromWhitelist=".
          json_encode($this->processUncoveredFilesFromWhitelist).
          "\n"
        ;

        // This alone will snag the file, and setup it's intiial state with the
        // platform as the container will run init()
        $fileStack = FileContainer::get($uncoveredFile);

      }
    }

  }

  /**
   * Returns the lines of a source file that should be ignored.
   *
   * @param string $filename
   *
   * @return array
   *
   * @throws InvalidArgumentException
   */
  private function getLinesToBeIgnored($filename) {

    if (!is_string($filename)) {
      throw InvalidArgumentException::create(1, 'string');
    }

    if (!isset($this->ignoredLines[$filename])) {

      $this->ignoredLines[$filename] = [];

      if ($this->disableIgnoredLines) {
        return $this->ignoredLines[$filename];
      }

      // --
      // JEO: ignored lines is a costly beast, we should consider disabling it,
      // but for now pull it off cache if possible.
      // --
      $linesToBeIgnored = Zynga_Source_Cache::getIgnoredLines($filename);

      if ($linesToBeIgnored !== null) {
        $this->ignoredLines[$filename] = $linesToBeIgnored;
        return $this->ignoredLines[$filename];
      }

      $ignore = false;
      $stop = false;
      $lines = file($filename);
      $numLines = count($lines);

      foreach ($lines as $index => $line) {
        if (!trim($line)) {
          $this->ignoredLines[$filename][] = $index + 1;
        }
      }

      $tokens = StreamCachingFactory::get($filename);

      $classes = array_merge(
        $tokens->getClasses()->toArray(),
        $tokens->getTraits()->toArray(),
      );

      $tokens = $tokens->tokens();
      $tokenCount = $tokens->count();

      for ($offset = 0; $offset < $tokenCount; $offset++) {
        $token = $tokens->get($offset);
        $tokenClass = get_class($token);

        if ($token instanceof PHP_Token_Comment ||
            $token instanceof PHP_Token_Doc_Comment) {

          if (!$token instanceof TokenInterface) {
            continue;
          }

          $_token = trim($token);
          $_line = trim($lines[$token->getLine() - 1]);

          if ($_token == '// @codeCoverageIgnore' ||
              $_token == '//@codeCoverageIgnore') {
            $ignore = true;
            $stop = true;
          } else if ($_token == '// @codeCoverageIgnoreStart' ||
                     $_token == '//@codeCoverageIgnoreStart') {
            $ignore = true;
          } else if ($_token == '// @codeCoverageIgnoreEnd' ||
                     $_token == '//@codeCoverageIgnoreEnd') {
            $stop = true;
          }

          if (!$ignore) {
            $start = $token->getLine();
            $end = $start + substr_count($token, "\n");

            // Do not ignore the first line when there is a token
            // before the comment
            if (0 !== strpos($_token, $_line)) {
              $start++;
            }

            for ($i = $start; $i < $end; $i++) {
              $this->ignoredLines[$filename][] = $i;
            }

            // A DOC_COMMENT token or a COMMENT token starting with "/*"
            // does not contain the final \n character in its text
            if (isset($lines[$i - 1]) &&
                0 === strpos($_token, '/*') &&
                '*/' === substr(trim($lines[$i - 1]), -2)) {
              $this->ignoredLines[$filename][] = $i;
            }
          }
          continue;
        }

        if ($token instanceof PHP_Token_Interface ||
            $token instanceof PHP_Token_Trait ||
            $token instanceof PHP_Token_Class ||
            $token instanceof PHP_Token_Function) {
          /* @var \PHP_Token_Interface $token */

          if (!$token instanceof TokenInterface) {
            continue;
          }

          $docblock = $token->getDocblock();

          $this->ignoredLines[$filename][] = $token->getLine();

          if (strpos($docblock, '@codeCoverageIgnore') ||
              ($this->ignoreDeprecatedCode &&
               strpos($docblock, '@deprecated'))) {
            $endLine = $token->getEndLine();

            for ($i = $token->getLine(); $i <= $endLine; $i++) {
              $this->ignoredLines[$filename][] = $i;
            }
          } else if ($token instanceof PHP_Token_Interface ||
                     $token instanceof PHP_Token_Trait ||
                     $token instanceof PHP_Token_Class) {

            if (empty($classes[$token->getName()]['methods'])) {
              for ($i = $token->getLine(); $i <= $token->getEndLine(); $i++) {
                $this->ignoredLines[$filename][] = $i;
              }
            } else {
              $firstMethod =
                array_shift($classes[$token->getName()]['methods']);

              do {
                $lastMethod =
                  array_pop($classes[$token->getName()]['methods']);
              } while ($lastMethod !== null &&
                       substr($lastMethod['signature'], 0, 18) == 'anonymous function');

              if ($lastMethod === null) {
                $lastMethod = $firstMethod;
              }

              for (
                $i = $token->getLine();
                $i < $firstMethod['startLine'];
                $i++
              ) {
                $this->ignoredLines[$filename][] = $i;
              }

              for (
                $i = $token->getEndLine();
                $i > $lastMethod['endLine'];
                $i--
              ) {
                $this->ignoredLines[$filename][] = $i;
              }
            }
          }

          continue;

        }

        if ($token instanceof PHP_Token_Enum) {
          for ($i = $token->getLine(); $i <= $token->getEndLine(); $i++) {
            $this->ignoredLines[$filename][] = $i;
          }
          continue;
        }

        if ($token instanceof PHP_Token_Namespace ||
            $token instanceof PHP_Token_Declare ||
            $token instanceof PHP_Token_Open_Tag ||
            $token instanceof PHP_Token_Close_Tag ||
            $token instanceof PHP_Token_Use) {

          if (!$token instanceof TokenInterface) {
            continue;
          }

          if ($token instanceof PHP_Token_Namespace) {
            $this->ignoredLines[$filename][] = $token->getEndLine();
          }

          if ($tokenClass instanceof PHP_Token_Use) {

            // look ahead for the semi colon
            $this->_handleMultiLineUseStatement(
              $filename,
              $tokens,
              $tokenCount,
              $offset,
            );

          } else {
            $this->ignoredLines[$filename][] = $token->getLine();
          }
          continue;
        }

        if ($ignore && $token instanceof TokenInterface) {
          $this->ignoredLines[$filename][] = $token->getLine();

          if ($stop) {
            $ignore = false;
            $stop = false;
          }
        }
      }

      $this->ignoredLines[$filename][] = $numLines + 1;

      $this->ignoredLines[$filename] =
        array_unique($this->ignoredLines[$filename]);

      sort($this->ignoredLines[$filename]);

      Zynga_Source_Cache::setIgnoredLines(
        $filename,
        $this->ignoredLines[$filename],
      );

    }

    return $this->ignoredLines[$filename];
  }

  private function _handleMultiLineUseStatement(
    $filename,
    $tokens,
    $tokenCount,
    $offset,
  ) {

    $targetFound = false;
    $futureOffset = $offset;
    $useStartsOnLine = 0;
    $useEndsOnLine = 0;

    while ($targetFound === false) {
      $futureToken = $tokens[$futureOffset];
      if ($futureToken instanceof PHP_Token_Semicolon) {
        $targetFound = true;
        $useEndsOnLine = $futureToken->getLine();
      } else {
        if ($useStartsOnLine === 0) {
          $useStartsOnLine = $futureToken->getLine();
        }
      }
      $futureOffset++;
    }

    // echo "useStartsOnLine: $useStartsOnLine\n";
    // echo "useEndsOnLine: $useEndsOnLine\n";

    for ($l = $useStartsOnLine; $l <= $useEndsOnLine; $l++) {
      // echo "  excusingLine: $l\n";
      $this->ignoredLines[$filename][] = $l;
    }

  }

  /**
   * @param array $data
   * @param array $linesToBeCovered
   * @param array $linesToBeUsed
   *
   * @throws UnintentionallyCoveredCodeException
   */
  private function performUnintentionallyCoveredCodeCheck(
    array $data,
    array $linesToBeCovered,
    array $linesToBeUsed,
  ) {
    $allowedLines = $this->getAllowedLines($linesToBeCovered, $linesToBeUsed);

    $unintentionallyCoveredUnits = [];

    foreach ($data as $file => $_data) {
      foreach ($_data as $line => $flag) {
        if ($flag == 1 && !isset($allowedLines[$file][$line])) {
          $unintentionallyCoveredUnits[] =
            $this->wizard->lookup($file, $line);
        }
      }
    }

    $unintentionallyCoveredUnits =
      $this->processUnintentionallyCoveredUnits($unintentionallyCoveredUnits);

    if (!empty($unintentionallyCoveredUnits)) {
      throw new UnintentionallyCoveredCodeException(
        $unintentionallyCoveredUnits,
      );
    }
  }

  /**
   * @param array $data
   * @param array $linesToBeCovered
   * @param array $linesToBeUsed
   *
   * @throws CoveredCodeNotExecutedException
   */
  private function performUnexecutedCoveredCodeCheck(
    array $data,
    array $linesToBeCovered,
    array $linesToBeUsed,
  ) {
    $expectedLines =
      $this->getAllowedLines($linesToBeCovered, $linesToBeUsed);

    foreach ($data as $file => $_data) {
      foreach (array_keys($_data) as $line) {
        if (!isset($expectedLines[$file][$line])) {
          continue;
        }

        unset($expectedLines[$file][$line]);
      }
    }

    $message = '';

    foreach ($expectedLines as $file => $lines) {
      if (empty($lines)) {
        continue;
      }

      foreach (array_keys($lines) as $line) {
        $message .= sprintf('- %s:%d', $file, $line).PHP_EOL;
      }
    }

    if (!empty($message)) {
      throw new CoveredCodeNotExecutedException($message);
    }
  }

  /**
   * @param array $linesToBeCovered
   * @param array $linesToBeUsed
   *
   * @return array
   */
  private function getAllowedLines(
    array $linesToBeCovered,
    array $linesToBeUsed,
  ) {
    $allowedLines = [];

    foreach (array_keys($linesToBeCovered) as $file) {
      if (!isset($allowedLines[$file])) {
        $allowedLines[$file] = [];
      }

      $allowedLines[$file] =
        array_merge($allowedLines[$file], $linesToBeCovered[$file]);
    }

    foreach (array_keys($linesToBeUsed) as $file) {
      if (!isset($allowedLines[$file])) {
        $allowedLines[$file] = [];
      }

      $allowedLines[$file] =
        array_merge($allowedLines[$file], $linesToBeUsed[$file]);
    }

    foreach (array_keys($allowedLines) as $file) {
      $allowedLines[$file] = array_flip(array_unique($allowedLines[$file]));
    }

    return $allowedLines;
  }

  /**
   * @return Driver
   *
   * @throws RuntimeException
   */
  private function selectDriver() {
    $runtime = new Runtime();

    if (!$runtime->canCollectCodeCoverage()) {
      throw new RuntimeException('No code coverage driver available');
    }

    // There is no real selection moment any more as we removed all the other
    // drivers.
    return new HHVM();

  }

  /**
   * @param array $unintentionallyCoveredUnits
   *
   * @return array
   */
  private function processUnintentionallyCoveredUnits(
    array $unintentionallyCoveredUnits,
  ) {
    $unintentionallyCoveredUnits = array_unique($unintentionallyCoveredUnits);
    sort($unintentionallyCoveredUnits);

    foreach (array_keys($unintentionallyCoveredUnits) as $k => $v) {
      $unit = explode('::', $unintentionallyCoveredUnits[$k]);

      if (count($unit) != 2) {
        continue;
      }

      $class = new \ReflectionClass($unit[0]);

      foreach ($this->unintentionallyCoveredSubclassesWhitelist as
               $whitelisted) {
        if ($class->isSubclassOf($whitelisted)) {
          unset($unintentionallyCoveredUnits[$k]);
          break;
        }
      }
    }

    return array_values($unintentionallyCoveredUnits);
  }

  private function initalizeData_DisplayPerFileData(string $message): void {
    // For debugging the initialization process.
    // echo $message;
  }

  /**
   * If we are processing uncovered files from whitelist,
   * we can initialize the data before we start to speed up the tests
   */
  private function initializeData(): void {
    $this->isInitialized = true;

    echo date('r')." - initalizeData - start\n";

    $fileCount = 0;

    foreach ($this->filter->getWhitelist() as $file) {

      $this->initalizeData_DisplayPerFileData("whiteListedFile=$file");

      if ($this->filter->isFiltered($file)) {
        $this->initalizeData_DisplayPerFileData(" - isFiltered\n");
        continue;
      }

      $fileCount++;

      $this->initalizeData_DisplayPerFileData(" - initalizing: ");

      $fileCoverage = array();

      $file = FileContainer::get($file);

      $this->initalizeData_DisplayPerFileData("done");

      $this->initalizeData_DisplayPerFileData("\n");

    }

    echo date('r')." - initalizeData - complete fileCount=$fileCount\n";

  }

}
