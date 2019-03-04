<?hh // strict

namespace Zynga\PHPUnit\V2;

use Zynga\PHPUnit\V2\Annotations;
use Zynga\PHPUnit\V2\Interfaces\TestInterface;
use Zynga\PHPUnit\V2\TestCase\OutputBuffer;
use Zynga\PHPUnit\V2\TestCase\Requirements;
use Zynga\PHPUnit\V2\TestCase\Size;
use Zynga\PHPUnit\V2\TestCase\Status;
use SebastianBergmann\Exporter\Exporter;
use SebastianBergmann\PHPUnit\Assertions;
use SebastianBergmann\PHPUnit\Exceptions\AssertionFailedException;
use SebastianBergmann\PHPUnit\Exceptions\ErrorException;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentException;
use SebastianBergmann\PHPUnit\Exceptions\TestError\IncompleteException;
use SebastianBergmann\PHPUnit\Exceptions\TestError\RiskyException;
use SebastianBergmann\PHPUnit\Exceptions\TestError\SkippedException;
use SebastianBergmann\PHPUnit\Exceptions\WarningException;
use Zynga\Framework\ReflectionCache\V1\ReflectionClasses;
use Zynga\Framework\Dynamic\V1\DynamicMethodCall;

// JEO: needs conversion.
use \PHPUnit_Framework_TestResult;

use \Throwable;
use \Exception;
use \ReflectionClass;
use \ReflectionMethod;

abstract class TestCase extends Assertions implements TestInterface {

  // The status for this test.
  private Status $_status;
  // The function name for the test
  private string $_name;
  // The class name for the test
  private string $_class;
  // output buffer management
  private OutputBuffer $_outputBuffer;
  // the test groups a testcase belongs to
  private Vector<string> $_groups;
  // expectedException* are a family of variables for providing assertion control
  // for a test that throws exceptions.
  private string $_expectedException;
  private string $_expectedExceptionMessage;
  private string $_expectedExceptionMessageRegExp;
  private int $_expectedExceptionCode;

  // expectedOutput are a family of variables for providing assertion control
  // for a test that does output to stdout.
  private ?string $_expectedOutput;
  private ?string $_expectedOutputRegExp;

  // test size
  private int $_size;
  // local local state, save upon set call.
  private Map<int, string> $_locale;
  // local ini settings, save upon set call
  private Map<string, string> $_iniSettings;
  // enables error handler conversion
  private bool $_useErrorHandler;
  // number of assertions fired by this test
  private int $_numAssertions;
  private Vector<string> $_dependencies;
  private bool $_hasDependencies;
  private ?PHPUnit_Framework_TestResult $_result;
  private ?PHPUnit_Framework_TestResult $_testResult;
  private Vector<mixed> $_data;
  private string $_dataName;

  public function __construct(
    string $name,
    mixed $data = null,
    string $dataName = '',
  ) {

    $this->_status = new Status();
    $this->_name = '';
    $this->_class = '';
    $this->_outputBuffer = new OutputBuffer();
    $this->_groups = Vector {};
    $this->_expectedException = '';
    $this->_expectedExceptionMessage = '';
    $this->_expectedExceptionMessageRegExp = '';
    $this->_expectedExceptionCode = -1;
    $this->_expectedOutput = null;
    $this->_expectedOutputRegExp = null;
    $this->_size = Size::UNDETERMINED;
    $this->_locale = Map {};
    $this->_iniSettings = Map {};
    $this->_useErrorHandler = false;
    $this->_numAssertions = 0;
    $this->_dependencies = Vector {};
    $this->_hasDependencies = false;
    $this->_result = null;
    $this->_testResult = null;
    $v_data = Vector {};
    if (is_array($data)) {
      $v_data->addAll($data);
    }
    $this->_data = $v_data;
    $this->_dataName = $dataName;

    $this->_name = $name;

  }

  final public function getTest(): TestCase {
    return $this;
  }

  /**
   * Returns a string representation of the test case.
   *
   * @return string
   */
  final public function toString(): string {

    $buffer = sprintf('%s::%s', $this->getClass(), $this->getName(false));

    return $buffer.$this->getDataSetAsString();

  }

  /**
   * Sets the name of a TestCase.
   *
   * @param  string
   */
  final public function setName(string $name): bool {
    $this->_name = $name;
    return true;
  }

  /**
   * Gets the name of a TestCase.
   *
   * @param bool $withDataSet
   *
   * @return string
   */
  final public function getName(bool $withDataSet = true): string {
    // @TODO: JEO we are not sure if we want to conitnue to support withDataSet as a param here.
    //if ($withDataSet) {
    //  return $this->_name.$this->getDataSetAsString(false);
    //} else {
    return $this->_name;
    //}
  }

  final public function getClass(): string {

    if ($this->_class != '') {
      return $this->_class;
    }

    $this->_class = get_class($this);
    return $this->_class;

  }

  final public function status(): Status {
    return $this->_status;
  }

  /**
   * Counts the number of test cases executed by run(TestResult result).
   *
   * @return int
   */
  final public function count(): int {
    return 1;
  }

  /**
   * Returns the annotations for this test.
   *
   * @return array
   *
   * @since Method available since Release 3.4.0
   */
  final public function getAnnotations(
  ): Map<string, Map<string, Vector<string>>> {

    $className = $this->getClass();
    $methodName = $this->getName(false);

    return Annotations::parseTestMethodAnnotations($className, $methodName);

  }

  final public function getAllAnnotationsForKey(string $key): Vector<string> {

    $collapsed = Vector {};

    $classValues = $this->getAnnotationsForKey('class', $key);

    foreach ($classValues as $classValue) {
      if (is_string($classValue)) {
        $collapsed->add($classValue);
      }
    }

    $methodValues = $this->getAnnotationsForKey('method', $key);

    foreach ($methodValues as $methodValue) {
      if (is_string($methodValue)) {
        $collapsed->add($methodValue);
      }
    }

    // return the stack of annotation combined
    return $collapsed;

  }

  final public function getAnnotationsForKey(
    string $context,
    string $key,
  ): Vector<string> {

    $allAnnotations = $this->getAnnotations();

    $contextAnnotations = $allAnnotations->get($context);

    if ($contextAnnotations instanceof Map) {

      $keyValue = $contextAnnotations->get($key);

      if ($keyValue instanceof Vector) {
        return $keyValue;
      }

    }

    // not found
    return Vector {};

  }

  /**
   * @return string
   *
   * @since Method available since Release 3.6.0
   */
  final public function getActualOutput(): string {
    return $this->_outputBuffer->getActualOutput();
  }

  /**
   * @since Method available since Release 4.2.0
   */
  final public function startOutputBuffering(): bool {
    return $this->_outputBuffer->startOutputBuffering();
  }

  /**
   * @since Method available since Release 4.2.0
   */
  final public function stopOutputBuffering(): bool {
    return $this->_outputBuffer->stopOutputBuffering();
  }

  /**
   * Returns the status of this test.
   *
   * @return int
   *
   * @since Method available since Release 3.1.0
   */
  final public function getStatus(): int {
    return $this->status()->getCode();
  }

  /**
   * Returns the status message of this test.
   *
   * @return string
   *
   * @since Method available since Release 3.3.0
   */
  final public function getStatusMessage(): string {
    return $this->status()->getMessage();
  }

  /**
   * @since Method available since Release 5.4.0
   */
  final public function getGroups(): Vector<string> {
    return $this->_groups;
  }

  /**
   * @param array $groups
   *
   * @since Method available since Release 5.4.0
   */
  final public function setGroups(mixed $groups): bool {
    // --
    // @TODO: When we have fully converted everything to hack we should move this
    //        function to support Vector<string>
    // --
    if ($groups instanceof Vector) {
      $this->_groups = $groups;
    }
    if (is_array($groups)) {
      $this->_groups = new Vector($groups);
    }
    return true;
  }

  /**
   * @since Method available since Release 3.4.0
   */
  final public function setExpectedExceptionFromAnnotation(): bool {

    $expectedExceptions =
      $this->getAnnotationsForKey('method', 'expectedException');

    foreach ($expectedExceptions as $expectedException) {
      $this->expectException(strval($expectedException));
    }

    $expectedExceptionMessages =
      $this->getAnnotationsForKey('method', 'expectedExceptionMessage');

    foreach ($expectedExceptionMessages as $expectedExceptionMessage) {
      $this->expectExceptionMessage(strval($expectedExceptionMessage));
    }

    $expectedExceptionMessageRegExps =
      $this->getAnnotationsForKey('method', 'expectedExceptionMessageRegExp');

    foreach ($expectedExceptionMessageRegExps as
             $expectedExceptionMessageRegExp) {
      $this->expectExceptionMessageRegExp(
        strval($expectedExceptionMessageRegExp),
      );
    }

    $expectedExceptionCodes =
      $this->getAnnotationsForKey('method', 'expectedExceptionCode');

    foreach ($expectedExceptionCodes as $expectedExceptionCode) {
      $this->expectExceptionCode(intval($expectedExceptionCode));
    }

    return true;

  }

  /**
   * @return string
   *
   * @since Method available since Release 3.2.0
   */
  final public function getExpectedException(): string {
    return $this->_expectedException;
  }

  /**
   * @param mixed      $exception
   * @param string     $message
   * @param int|string $code
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since      Method available since Release 3.2.0
   * @deprecated Method deprecated since Release 5.2.0
   */
  final public function setExpectedException(
    string $exception,
    string $message = '',
    int $code = -1,
  ): bool {
    $this->_expectedException = $exception;

    if ($message !== '') {
      $this->expectExceptionMessage($message);
    }

    $this->expectExceptionCode($code);

    return true;
  }

  /**
   * @param mixed  $exception
   * @param string $messageRegExp
   * @param int    $code
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since Method available since Release 4.3.0
   */
  final public function setExpectedExceptionRegExp(
    string $exception,
    string $messageRegExp = '',
    int $code = -1,
  ): bool {

    $this->expectException($exception);
    $this->expectExceptionMessageRegExp($messageRegExp);
    $this->expectExceptionCode($code);

    return true;
  }

  /**
   * @param mixed $exception
   *
   * @since Method available since Release 5.2.0
   */
  final public function expectException(string $exception): bool {
    $this->_expectedException = $exception;
    return true;
  }

  /**
   * @param int|string $code
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since Method available since Release 5.2.0
   */
  final public function expectExceptionCode(int $code): bool {
    $this->_expectedExceptionCode = $code;
    return true;
  }

  final public function getExpectedExceptionCode(): int {
    return $this->_expectedExceptionCode;
  }

  /**
   * @param string $message
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since Method available since Release 5.2.0
   */
  final public function expectExceptionMessage(string $message): bool {
    $this->_expectedExceptionMessage = $message;
    return true;
  }

  final public function getExpectedExceptionMessage(): string {
    return $this->_expectedExceptionMessage;
  }

  /**
   * @param string $messageRegExp
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since Method available since Release 5.2.0
   */
  final public function expectExceptionMessageRegExp(
    string $messageRegExp,
  ): bool {
    $this->_expectedExceptionMessageRegExp = $messageRegExp;
    return true;
  }

  final public function getExpectedExceptionMessageRegExp(): string {
    return $this->_expectedExceptionMessageRegExp;
  }

  /**
   * Returns the size of the test.
   *
   * @return int
   *
   * @since Method available since Release 3.6.0
   */
  final public function getSize(): int {
    $this->setSizeFromAnnotation();
    return $this->_size;
  }

  final public function setSizeFromAnnotation(): bool {

    if ($this->_size != Size::UNDETERMINED) {
      return true;
    }

    $groups = $this->getGroups();

    foreach ($groups as $group) {
      if ($group == 'large') {
        $this->_size = Size::LARGE;
        return true;
      }
      if ($group == 'medium') {
        $this->_size = Size::MEDIUM;
        return true;
      }
      if ($group == 'small') {
        $this->_size = Size::SMALL;
        return true;
      }
    }

    $annotations = $this->getAnnotations();

    $methodAnnotations = $annotations->get('method');

    if (!$methodAnnotations instanceof Map) {
      return false;
    }

    $large = $methodAnnotations->get('large');

    if ($large !== null) {
      $this->_size = Size::LARGE;
      return true;
    }

    $medium = $methodAnnotations->get('medium');

    if ($medium !== null) {
      $this->_size = Size::MEDIUM;
      return true;
    }

    $small = $methodAnnotations->get('small');

    if ($small !== null) {
      $this->_size = Size::SMALL;
      return true;
    }

    $this->_size = Size::UNKNOWN;

    return true;

  }

  /**
   * @return bool
   *
   * @since Method available since Release 5.3.4
   */
  final public function hasSize(): bool {
    return $this->getSize() !== Size::UNKNOWN;
  }

  /**
   * @return bool
   *
   * @since Method available since Release 5.3.4
   */
  final public function isSmall(): bool {
    return $this->getSize() === Size::SMALL;
  }

  /**
   * @return bool
   *
   * @since Method available since Release 5.3.4
   */
  final public function isMedium(): bool {
    return $this->getSize() === Size::MEDIUM;
  }

  /**
   * @return bool
   *
   * @since Method available since Release 5.3.4
   */
  final public function isLarge(): bool {
    return $this->getSize() === Size::LARGE;
  }

  /**
   * This method is a wrapper for the setlocale() function that automatically
   * resets the locale to its original value after the test is run.
   *
   * @param int    $category
   * @param string $locale
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since Method available since Release 3.1.0
   */
  final public function setLocale(int $category, string $locale): bool {

    $categories = array(
      LC_ALL,
      LC_COLLATE,
      LC_CTYPE,
      LC_MONETARY,
      LC_NUMERIC,
      LC_TIME,
    );

    if (defined('LC_MESSAGES')) {
      $categories[] = LC_MESSAGES;
    }

    if (!in_array($category, $categories)) {
      throw new InvalidArgumentException('category='.$category.' is invalid');
    }

    $this->_locale->set($category, setlocale($category, 0));

    $localeArgs = array($category, $locale);

    $result = call_user_func_array('setlocale', $localeArgs);

    if ($result === false) {
      throw new ErrorException(
        'The locale functionality is not implemented on your platform, '.
        'the specified locale does not exist or the category name is '.
        'invalid.',
        -888, // JEO: the dreaded 888
        __FILE__,
        __LINE__,
      );
    }

    return true;

  }

  final public function getLocales(): Map<int, string> {
    return $this->_locale;
  }

  final public function clearLocales(): bool {

    foreach ($this->getLocales() as $category => $locale) {
      setlocale($category, $locale);
    }

    $this->_locale->clear();

    return true;

  }

  /**
   * This method is a wrapper for the ini_set() function that automatically
   * resets the modified php.ini setting to its original value after the
   * test is run.
   *
   * @param string $varName
   * @param string $newValue
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since Method available since Release 3.0.0
   */
  final public function iniSet(string $varName, string $newValue): bool {

    $currentValue = ini_set($varName, $newValue);

    if ($currentValue !== false) {
      $this->_iniSettings->set($varName, $currentValue);
    } else {
      throw new ErrorException(
        sprintf(
          'INI setting "%s" could not be set to "%s".',
          $varName,
          $newValue,
        ),
        -888,
        __FILE__,
        __LINE__,
      );
    }

    return true;

  }

  final public function getIniSettings(): Map<string, string> {
    return $this->_iniSettings;
  }

  final public function clearIniSettings(): bool {

    foreach ($this->getIniSettings() as $varName => $oldValue) {
      ini_set($varName, $oldValue);
    }

    $this->_iniSettings->clear();

    return true;
  }

  /**
   * @param string $expectedRegex
   *
   * @since Method available since Release 3.6.0
   *
   * @throws PHPUnit_Framework_Exception
   */
  final public function expectOutputRegex(string $expectedRegex): bool {
    $this->_expectedOutputRegExp = $expectedRegex;
    return true;
  }

  final public function getExpectedOutputRegex(): ?string {
    return $this->_expectedOutputRegExp;
  }

  /**
   * @param string $expectedString
   *
   * @since Method available since Release 3.6.0
   */
  final public function expectOutputString(string $expectedString): bool {
    $this->_expectedOutput = $expectedString;
    return true;

  }

  final public function getExpectedOutput(): ?string {
    return $this->_expectedOutput;
  }

  /**
   * @return bool
   *
   * @since Method available since Release 4.3.3
   */
  final public function hasExpectationOnOutput(): bool {
    return
      is_string($this->_expectedOutput) ||
      is_string($this->_expectedOutputRegExp);
  }

  /**
   * @return bool
   *
   * @since Method available since Release 3.6.5
   * @deprecated
   */
  final public function hasPerformedExpectationsOnOutput(): bool {
    return $this->hasExpectationOnOutput();
  }

  /**
   * @return bool
   *
   * @since Method available since Release 3.6.0
   */
  final public function hasOutput(): bool {

    if (strlen($this->getActualOutput()) === 0) {
      return false;
    }

    if ($this->hasExpectationOnOutput()) {
      return false;
    }

    return true;
  }

  /**
   * @param bool $useErrorHandler
   *
   * @since Method available since Release 3.4.0
   */
  final public function setUseErrorHandler(bool $useErrorHandler): bool {
    $this->_useErrorHandler = $useErrorHandler;
    return true;
  }

  final public function getUseErrorHandler(): bool {
    return $this->_useErrorHandler;
  }

  /**
   * @since Method available since Release 3.4.0
   */
  final public function setUseErrorHandlerFromAnnotation(): bool {

    $annotations = $this->getAnnotations();

    $classAnnotations = $annotations->get('class');

    if ($classAnnotations instanceof Map) {
      $useErrorHandler = $classAnnotations->get('errorHandler');
      if ($useErrorHandler !== null) {
        $this->setUseErrorHandler(boolval($useErrorHandler));
        return true;
      }
    }

    $methodAnnotations = $annotations->get('method');

    if ($methodAnnotations instanceof Map) {
      $useErrorHandler = $methodAnnotations->get('errorHandler');
      if ($useErrorHandler !== null) {
        $this->setUseErrorHandler(boolval($useErrorHandler));
        return true;
      }
    }

    return true;

  }

  /**
   * Returns whether or not this test has failed.
   *
   * @return bool
   *
   * @since Method available since Release 3.0.0
   */
  final public function hasFailed(): bool {

    $status = $this->getStatus();

    return
      $status == Status::STATUS_FAILURE || $status == Status::STATUS_ERROR;

  }

  /**
   * Adds a value to the assertion counter.
   *
   * @param int $count
   *
   * @since Method available since Release 3.3.3
   */
  final public function addToAssertionCount(int $count): bool {
    $this->_numAssertions += $count;
    return true;
  }

  /**
   * Returns the number of assertions performed by this test.
   *
   * @return int
   *
   * @since Method available since Release 3.3.0
   */
  final public function getNumAssertions(): int {
    return $this->_numAssertions;
  }

  /**
   * @since Method available since Release 3.6.0
   */
  final protected function checkRequirements(): void {

    $name = $this->getName(false);

    // if the test moethod doesn't exist stop.
    if (!$name || !method_exists($this, $name)) {
      return;
    }

    $missingRequirements = Requirements::getMissingRequirements($this);

    if ($missingRequirements->count() > 0) {
      $this->markTestSkipped(implode(PHP_EOL, $missingRequirements));
    }

  }

  /**
   * Sets the dependencies of a TestCase.
   *
   * @param array $dependencies
   *
   * @since Method available since Release 3.4.0
   */
  final public function setDependencies(Vector<string> $dependencies): bool {
    $this->_dependencies = $dependencies;
    $this->_hasDependencies = $dependencies->count() > 0;
    return true;
  }

  final public function getDependencies(): Vector<string> {
    return $this->_dependencies;
  }

  /**
   * Returns true if the tests has dependencies
   *
   * @return bool
   *
   * @since Method available since Release 4.0.0
   */
  final public function hasDependencies(): bool {
    return $this->_hasDependencies;
  }

  /**
   * Creates a default TestResult object.
   *
   * @return PHPUnit_Framework_TestResult
   */
  final public function createResult(): PHPUnit_Framework_TestResult {
    return new PHPUnit_Framework_TestResult();
  }

  /**
   * @param mixed $result
   *
   * @since Method available since Release 3.4.0
   */
  final public function setResult(
    ?PHPUnit_Framework_TestResult $result,
  ): bool {
    $this->_testResult = $result;
    return true;
  }

  final public function getResult(): PHPUnit_Framework_TestResult {
    if ($this->_testResult == null) {
      $this->_testResult = $this->createResult();
    }
    return $this->_testResult;
  }

  /**
   * @param PHPUnit_Framework_TestResult $result
   *
   * @since Method available since Release 3.6.0
   */
  final public function setTestResultObject(
    PHPUnit_Framework_TestResult $result,
  ): bool {
    $this->_result = $result;
    return true;
  }

  /**
   * @return PHPUnit_Framework_TestResult
   *
   * @since Method available since Release 3.5.7
   */
  final public function getTestResultObject(): ?PHPUnit_Framework_TestResult {
    return $this->_result;
  }

  final public function freeResult(): bool {
    $this->_result = null;
    return true;
  }

  final public function getDataName(): string {
    return $this->_dataName;
  }

  /**
   * @return string
   *
   * @since Method available since Release 5.4.0
   */
  final public function dataDescription(): string {
    return $this->getDataName();
  }

  final public function getData(): Vector<mixed> {
    return $this->_data;
  }

  /**
   * Gets the data set description of a TestCase.
   *
   * @param bool $includeData
   *
   * @return string
   *
   * @since Method available since Release 3.3.0
   */
  final public function getDataSetAsString(bool $includeData = true): string {
    $buffer = '';

    if ($this->usesDataProvider()) {
      $buffer .= sprintf(' with data set "%s"', $this->getDataName());

      $exporter = new Exporter();

      if ($includeData) {
        $buffer .= sprintf(
          ' (%s)',
          $exporter->shortenedRecursiveExport($this->getData()),
        );
      }
    }

    return $buffer;
  }

  /**
   * @return bool
   *
   * @since Method available since Release 5.4.0
   */
  final public function usesDataProvider(): bool {
    if ($this->_data->count() > 0) {
      return true;
    }
    return false;
  }

  /**
   * @since Method available since Release 3.5.4
   */
  final public function handleDependencies(): bool {

    if ($this->hasDependencies()) {

      $className = $this->getClass();
      $tr = $this->getTestResultObject();

      $passed = array();
      if ($tr instanceof PHPUnit_Framework_TestResult) {
        $passed = $tr->passed();
      }

      $passedKeys = array_keys($passed);
      $numKeys = count($passedKeys);

      for ($i = 0; $i < $numKeys; $i++) {
        $pos = strpos($passedKeys[$i], ' with data set');

        if ($pos !== false) {
          $passedKeys[$i] = substr($passedKeys[$i], 0, $pos);
        }
      }

      $passedKeys = array_flip(array_unique($passedKeys));

      foreach ($this->getDependencies() as $dependency) {
        $clone = false;

        if (strpos($dependency, 'clone ') === 0) {
          $clone = true;
          $dependency = substr($dependency, strlen('clone '));
        } else if (strpos($dependency, '!clone ') === 0) {
          $clone = false;
          $dependency = substr($dependency, strlen('!clone '));
        }

        if (strpos($dependency, '::') === false) {
          $dependency = $className.'::'.$dependency;
        }

        if (!array_key_exists($dependency, $passedKeys)) {
          $this->getResult()->addError(
            $this,
            new SkippedException(
              sprintf('This test depends on "%s" to pass.', $dependency),
            ),
            0.0,
          );

          return false;
        }

        if (array_key_exists($dependency, $passed)) {
          if ($passed[$dependency]['size'] != Size::UNKNOWN &&
              $this->getSize() != Size::UNKNOWN &&
              $passed[$dependency]['size'] > $this->getSize()) {
            $this->getResult()->addError(
              $this,
              new SkippedException(
                'This test depends on a test that is larger than itself.',
              ),
              0.0,
            );

            return false;
          }
        }
      }
    }

    return true;
  }

  /**
   * Runs the test case and collects the results in a TestResult object.
   * If no TestResult object is passed a new one will be created.
   *
   * @param PHPUnit_Framework_TestResult $result
   *
   * @return PHPUnit_Framework_TestResult
   *
   * @throws PHPUnit_Framework_Exception
   */
  final public function run(
    ?PHPUnit_Framework_TestResult $result = null,
  ): PHPUnit_Framework_TestResult {

    if ($result === null) {
      $result = $this->createResult();
    }

    // WarningTestCase is probably dead now that Mocks are dead.
    // if (!$this instanceof PHPUnit_Framework_WarningTestCase) {
    $this->setTestResultObject($result);
    $this->setUseErrorHandlerFromAnnotation();
    //}

    $oldErrorHandlerSetting = null;

    if ($this->getUseErrorHandler() !== null) {
      $oldErrorHandlerSetting = $result->getConvertErrorsToExceptions();
      $result->convertErrorsToExceptions($this->getUseErrorHandler());
    }

    // if (!$this instanceof PHPUnit_Framework_WarningTestCase &&
    if (!$this->handleDependencies()) {
      return $result;
    }

    // Pass on the test into the run function for result.
    $test = $this;
    if ($test instanceof TestInterface) {
      $result->run($test);
    }

    if (is_bool($oldErrorHandlerSetting)) {
      $result->convertErrorsToExceptions($oldErrorHandlerSetting);
    }

    $this->freeResult();

    return $result;

  }

  final private function runTest_Handle_Exception_Trapping(
    ?Exception $e,
  ): bool {

    $expectedException = $this->getExpectedException();

    if ($expectedException == '') {
      return true;
    }

    if (!$e instanceof Exception) {
      $this->fail(
        'Expected='.$expectedException.' was presented e='.json_encode($e),
      );
      return false;
    }

    try {

      $this->assertExceptionSameClass($expectedException, $e);

      $expectedExceptionMessage = $this->getExpectedExceptionMessage();
      $expectedExceptionMessageRegExp =
        $this->getExpectedExceptionMessageRegExp();

      if ($expectedExceptionMessage != '') {
        $this->assertExceptionSameMessage($expectedExceptionMessage, $e);
      }

      if ($expectedExceptionMessageRegExp != '') {
        $this->assertExceptionRegexpMessage(
          $expectedExceptionMessageRegExp,
          $e,
        );
      }

      if ($this->getExpectedExceptionCode() !== -1) {
        $this->assertEquals($this->getExpectedExceptionCode(), $e->getCode());
      }

      return true;

    } catch (AssertionFailedException $expectedError) {

      $this->fail($expectedError->getMessage());
      return false;

    }

    return true;

  }

  /**
   * Override to run the test and assert its state.
   *
   * @return mixed
   *
   * @throws Exception|PHPUnit_Framework_Exception
   * @throws PHPUnit_Framework_Exception
   */
  final public function runTest(): mixed {

    $testName = $this->getName(false);

    $e = null;
    $testResult = null;

    try {
      $class = ReflectionClasses::getReflection($this);

      if ($class instanceof ReflectionClass) {
        $method = $class->getMethod($testName);
        $testArguments = $this->getData();
        $testResult = $method->invokeArgs($this, $testArguments->toArray());
      } else {
        $this->fail('ReflectionError class='.$class);
      }
    } catch (Exception $_e) {
      $e = $_e;
    }

    // --
    // Handle user space exception trapping, if it's not handled / failed
    // return void / null.
    // --
    if ($this->runTest_Handle_Exception_Trapping($e) == true) {
      // We have handled the exception.
      return null;
    } else if ($e instanceof Exception) {
      // So we have a non-monitored exception, re-throw.
      throw $e;
    }

    return $testResult;

  }

  final public function mergeHooks(
    Vector<string> $template,
    Vector<string> $userDefined,
  ): Vector<string> {

    $combinedMap = Map {};

    foreach ($template as $hook) {
      $combinedMap->set($hook, true);
    }

    foreach ($userDefined as $hook) {
      $combinedMap->set($hook, true);
    }

    return $combinedMap->keys();

  }

  final public function getHookMethods(): Map<string, Vector<string>> {

    $beforeClassMethods = $this->mergeHooks(
      Vector {'setUpBeforeClass'},
      $this->getAllAnnotationsForKey('beforeClass'),
    );

    $afterClassMethods = $this->mergeHooks(
      Vector {'tearDownAfterClass'},
      $this->getAllAnnotationsForKey('afterClass'),
    );

    $beforeMethods = $this->mergeHooks(
      Vector {'setUp'},
      $this->getAllAnnotationsForKey('before'),
    );

    $afterMethods = $this->mergeHooks(
      Vector {'tearDown'},
      $this->getAllAnnotationsForKey('after'),
    );

    $hooks = Map {
      'beforeClass' => $beforeClassMethods,
      'before' => $beforeMethods,
      'after' => $afterMethods,
      'afterClass' => $afterClassMethods,
    };

    return $hooks;

  }

  /**
   * Runs the bare test sequence.
   */
  final public function runBare(): void {

    $this->startOutputBuffering();

    $currentWorkingDirectory = getcwd();

    $hookMethods = $this->getHookMethods();

    $e = null;
    $_e = null;
    $hasMetRequirements = false;

    try {

      $this->checkRequirements();

      $hasMetRequirements = true;

      $this->setExpectedExceptionFromAnnotation();

      foreach ($hookMethods['before'] as $method) {
        // JEO: make this be dynamic calling.
        DynamicMethodCall::callMethodOnObject($this, $method, Vector {});
      }

      $this->assertPreConditions();

      $t_result = $this->runTest();

      if ($t_result instanceof PHPUnit_Framework_TestResult) {
        $this->setResult($t_result);
      }

      $this->assertPostConditions();

      $this->status()->setMessageAndCode('', Status::STATUS_PASSED);

    } catch (IncompleteException $e) {
      $this->status()
        ->setMessageAndCode($e->getMessage(), Status::STATUS_INCOMPLETE);
    } catch (SkippedException $e) {
      $this->status()
        ->setMessageAndCode($e->getMessage(), Status::STATUS_SKIPPED);
    } catch (WarningException $e) {
      $this->status()
        ->setMessageAndCode($e->getMessage(), Status::STATUS_WARNING);
    } catch (AssertionFailedException $e) {
      $this->status()
        ->setMessageAndCode($e->getMessage(), Status::STATUS_FAILURE);
    } catch (Exception $_e) {
      $e = $_e;
    }

    if ($_e instanceof Exception) {
      $this->status()
        ->setMessageAndCode($_e->getMessage(), Status::STATUS_ERROR);
    }

    // Tear down the fixture. An exception raised in tearDown() will be
    // caught and passed on when no exception was raised before.
    try {
      if ($hasMetRequirements) {
        foreach ($hookMethods['after'] as $method) {
          DynamicMethodCall::callMethodOnObject($this, $method, Vector {});
        }
      }
    } catch (Exception $_e) {
      if ($e == null) {
        $e = $_e;
      }
    }

    try {
      $this->stopOutputBuffering();
    } catch (RiskyException $_e) {
      if ($e == null) {
        $e = $_e;
      }
    }

    if ($currentWorkingDirectory != getcwd()) {
      chdir($currentWorkingDirectory);
    }

    // Clean up INI settings.
    $this->clearIniSettings();

    // Clean up locale settings.
    $this->clearLocales();

    // Perform assertion on output.

    try {
      $output = $this->getActualOutput();

      $expectedOutputRegex = $this->getExpectedOutputRegex();
      $expectedOutput = $this->getExpectedOutput();

      // var_dump(get_class($this));
      // var_dump('expectedOutputRegex');
      // var_dump($expectedOutputRegex);
      // var_dump('output');
      // var_dump($output);
      // var_dump('output-END');

      if (is_string($expectedOutputRegex)) {
        $this->assertRegExp($expectedOutputRegex, $output);
      } else if (is_string($expectedOutput)) {
        $this->assertEquals($expectedOutput, $output);
      }

    } catch (AssertionFailedException $_e) {
      // Any asserts at this point are for output trapping.
      $e = $_e;
      $this->status()
        ->setMessageAndCode($e->getMessage(), Status::STATUS_FAILURE);
    } catch (Exception $_e) {
      $e = $_e;
    }

    // Workaround for missing "finally".
    if ($e instanceof Exception) {
      $this->onNotSuccessfulTest($e);
    }
  }

  /**
   * This method is called when a test method did not execute successfully.
   *
   * @param Exception|Throwable $e
   *
   * @since Method available since Release 3.4.0
   *
   * @throws Exception|Throwable
   */
  final public function onNotSuccessfulTest(Exception $e): void {
    throw $e;
  }

  // --
  // User hookable / definable functions start here.
  // --

  /**
   * This method is called before the first test of this test class is run.
   *
   * @since Method available since Release 3.4.0
   */
  public function setUpBeforeClass(): void {}

  /**
   * Sets up the fixture, for example, open a network connection.
   * This method is called before a test is executed.
   */
  public function setUp(): void {}

  /**
   * Performs assertions shared by all tests of a test case.
   *
   * This method is called before the execution of a test starts
   * and after setUp() is called.
   *
   * @since Method available since Release 3.2.8
   */
  public function assertPreConditions(): void {}

  /**
   * Performs assertions shared by all tests of a test case.
   *
   * This method is called before the execution of a test ends
   * and before tearDown() is called.
   *
   * @since Method available since Release 3.2.8
   */
  public function assertPostConditions(): void {}

  /**
   * Tears down the fixture, for example, close a network connection.
   * This method is called after a test is executed.
   */
  public function tearDown(): void {}

  /**
   * This method is called after the last test of this test class is run.
   *
   * @since Method available since Release 3.4.0
   */
  public function tearDownAfterClass(): void {}

}
