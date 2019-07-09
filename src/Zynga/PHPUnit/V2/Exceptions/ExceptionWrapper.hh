<?hh // strict

namespace Zynga\PHPUnit\V2\Exceptions;

use Zynga\PHPUnit\V2\Exceptions\Exception as ExceptionBase;
use Zynga\PHPUnit\V2\Exceptions\ExceptionUtil;

use \Exception;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

/**
 * Wraps Exceptions thrown by code under test.
 *
 * Re-instantiates Exceptions thrown by user-space code to retain their original
 * class names, properties, and stack traces (but without arguments).
 *
 * Unlike PHPUnit_Framework_Exception, the complete stack of previous Exceptions
 * is processed.
 *
 * @since Class available since Release 4.3.0
 */
class ExceptionWrapper extends ExceptionBase {

  protected string $classname;
  protected ?ExceptionWrapper $previousException;

  /**
   * @param Throwable|Exception $e
   */
  public function __construct(Exception $e) {
    // PDOException::getCode() is a string.
    // @see http://php.net/manual/en/class.pdoexception.php#95812
    parent::__construct($e->getMessage(), (int) $e->getCode());

    $this->classname = get_class($e);
    $this->file = $e->getFile();
    $this->line = $e->getLine();

    $previousException = $e->getPrevious();
    if ($previousException instanceof Exception) {
      $this->previousException = new self($previousException);
    }

  }

  /**
   * @return string
   */
  public function getClassname(): string {
    return $this->classname;
  }

  /**
   * @return PHPUnit_Framework_ExceptionWrapper
   */
  public function getPreviousWrapped(): ?ExceptionWrapper {
    return $this->previousException;
  }

  /**
   * @return string
   */
  public function __toString(): string {
    $string = ExceptionUtil::exceptionToString($this);
    $string .= $this->getTraceAsString();

    if ($this->previous) {
      $string .= "\nCaused by\n".$this->previous;
    }

    return $string;
  }

}
