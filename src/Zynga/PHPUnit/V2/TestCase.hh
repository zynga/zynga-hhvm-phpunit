<?hh // strict

namespace Zynga\PHPUnit\V2;

use Zynga\PHPUnit\V2\Annotations;
use Zynga\PHPUnit\V2\TestCase\OutputBuffer;
use Zynga\PHPUnit\V2\TestCase\Status;
use SebastianBergmann\PHPUnit\Assertions;

class TestCase extends Assertions {

  private Status $_status;
  private string $_name;
  private string $_class;
  private OutputBuffer $_outputBuffer;
  private Vector<string> $_groups;
  private string $_expectedException;
  private string $_expectedExceptionMessage;
  private string $_expectedExceptionMessageRegExp;
  private int $_expectedExceptionCode;

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
  final public function getAnnotations(): Map<string, Map<string, string>> {

    $className = $this->getClass();
    $methodName = $this->getName(false);

    //var_dump('className='.$className);
    //var_dump('methodName='.$methodName);

    return Annotations::parseTestMethodAnnotations($className, $methodName);

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

    $annotations = $this->getAnnotations();

    // echo "annotations=\n";
    // var_dump($annotations);

    $methodAnnotations = $annotations->get('method');

    if (!$methodAnnotations instanceof Map) {
      return false;
    }

    $expectedException = $methodAnnotations->get('expectedException');

    if ($expectedException != null) {
      $this->expectException(strval($expectedException));
    }

    $expectedExceptionMessage =
      $methodAnnotations->get('expectedExceptionMessage');

    if ($expectedExceptionMessage != null) {
      $this->expectExceptionMessage(strval($expectedExceptionMessage));
    }

    $expectedExceptionMessageRegExp =
      $methodAnnotations->get('expectedExceptionMessageRegExp');

    if ($expectedExceptionMessageRegExp != null) {
      $this->expectExceptionMessageRegExp(
        strval($expectedExceptionMessageRegExp),
      );
    }

    $expectedExceptionCode = $methodAnnotations->get('expectedExceptionCode');

    if ($expectedExceptionCode != null) {
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
