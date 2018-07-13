<?hh // partial

// --
// JEO: For some reason its not able to find PHPUnit_Util_Filter, so no strict
// for now.
// --

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace PHPUnit\Exceptions;

use PHPUnit\Exceptions\Exception as PHPUnit_Exceptions_Exception;

use \Exception;
use \PHPUnit_Util_Filter;
use \PHPUnit_Framework_TestFailure;

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
class ExceptionWrapper extends PHPUnit_Exceptions_Exception {
    /**
     * @var string
     */
    protected string $classname;

    /**
     * @var ExceptionWrapper|null
     */
    protected ?ExceptionWrapper $_previous;

    /**
     * @param Throwable|Exception $e
     */
    public function __construct(Exception $e) {
        // PDOException::getCode() is a string.
        // @see http://php.net/manual/en/class.pdoexception.php#95812
        parent::__construct($e->getMessage(), (int) $e->getCode());

        $this->classname = get_class($e);
        $this->file      = $e->getFile();
        $this->line      = $e->getLine();

        $eTrace = $e->getTrace();

        foreach ( $eTrace as $frame ) {
          unset($frame['args']);
          $this->serializableTrace->add($frame);
        }

        $previous = $e->getPrevious();

        if ($previous !== null) {
            $this->_previous = new self($previous);
        }

    }

    /**
     * @return string
     */
    public function getClassname(): string {
        return $this->classname;
    }

    /**
     * @return ExceptionWrapper
     */
    public function getPreviousWrapped(): ?ExceptionWrapper {
      return $this->_previous;
    }

    /**
     * @return string
     */
    public function __toString(): string
    {
        $string = PHPUnit_Framework_TestFailure::exceptionToString($this);

        if ($trace = PHPUnit_Util_Filter::getFilteredStacktrace($this)) {
            $string .= "\n" . $trace;
        }

        if ($this->previous) {
            $string .= "\nCaused by\n" . $this->previous;
        }

        return $string;
    }
}
