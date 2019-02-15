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

  public function __construct(string $name = '') {

    $this->_status = new Status();
    $this->_name = '';
    $this->_class = '';
    $this->_outputBuffer = new OutputBuffer();

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

    var_dump('className='.$className);
    var_dump('methodName='.$methodName);

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
