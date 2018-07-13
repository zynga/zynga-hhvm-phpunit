<?hh // strict

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace PHPUnit\Exceptions;

/**
 * Base class for all PHPUnit Framework exceptions.
 *
 * Ensures that exceptions thrown during a test run do not leave stray
 * references behind.
 *
 * Every Exception contains a stack trace. Each stack frame contains the 'args'
 * of the called function. The function arguments can contain references to
 * instantiated objects. The references prevent the objects from being
 * destructed (until test results are eventually printed), so memory cannot be
 * freed up.
 *
 * With enabled process isolation, test results are serialized in the child
 * process and unserialized in the parent process. The stack trace of Exceptions
 * may contain objects that cannot be serialized or unserialized (e.g., PDO
 * connections). Unserializing user-space objects from the child process into
 * the parent would break the intended encapsulation of process isolation.
 *
 * @see http://fabien.potencier.org/article/9/php-serialization-stack-traces-and-exceptions
 * @since Class available since Release 3.4.0
 */
use \RuntimeException;
use \Exception as BaseException;

use PHPUnit\Framework\TestFailure;
use PHPUnit\Interfaces\Exception as ExceptionInterface;
use PHPUnit\Util\FilterUtil;

use \PHPUnit_Framework_TestFailure;

class Exception extends RuntimeException implements ExceptionInterface {
    /**
     * @var array
     */
    protected Vector<mixed> $serializableTrace;

    public function __construct(string $message = '', int $code = 0, ?BaseException $previous = null) {

        parent::__construct($message, $code, $previous);

        $this->serializableTrace = Vector {};

        $trace = $this->getTrace();

        foreach ($trace as $i => $call) {

          // strip the function arguments off the call
          unset($call['args']);

          $this->serializableTrace->add($call);

        }

    }

    /**
     * Returns the serializable trace (without 'args').
     *
     * @return array
     */
    public function getSerializableTrace(): Vector<mixed> {
      return $this->serializableTrace;
    }

    /**
     * @return string
     */
    public function __toString(): string {

        $string = TestFailure::exceptionToString($this);

        if ($trace = FilterUtil::getFilteredStacktrace($this)) {
            $string .= "\n" . $trace;
        }

        return $string;
    }

    public function __sleep(): mixed {
      $objectVars = get_object_vars($this);
      if ( is_array($objectVars) ) {
        return array_keys($objectVars);
      }
      return null;
    }
}
