<?hh // strict

namespace Zynga\PHPUnit\V2;

use Zynga\PHPUnit\V2\Annotations;
use Zynga\PHPUnit\V2\TestCase\OutputBuffer;
use Zynga\PHPUnit\V2\TestCase\Requirements;
use Zynga\PHPUnit\V2\TestCase\Size;
use Zynga\PHPUnit\V2\TestCase\Status;
use SebastianBergmann\PHPUnit\Assertions;
use SebastianBergmann\PHPUnit\Exceptions\ErrorException;
use SebastianBergmann\PHPUnit\Exceptions\InvalidArgumentException;

class TestCase extends Assertions {

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

  public function __construct(string $name = '') {

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

    if ($name != '') {
      $this->_name = $name;
    }

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

    //var_dump('className='.$className);
    //var_dump('methodName='.$methodName);

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
    //var_dump(__LINE__ . ' getStatusMessage: ' . $this->status()->getMessage());
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

    // echo "annotations=\n";
    // var_dump($annotations);

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
    // if ($this->outputExpectedString !== null) {
    //   throw new PHPUnit_Framework_Exception();
    // }
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
    // if ($this->outputExpectedRegex !== null) {
    //   throw new PHPUnit_Framework_Exception();
    // }
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

    // echo "annotations=\n";
    // var_dump($annotations);

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
