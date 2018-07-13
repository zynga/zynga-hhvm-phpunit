<?hh // strict

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace PHPUnit\Framework;

use PHPUnit\Exceptions\Exception as PHPUnit_Exceptions_Exception;
use PHPUnit\Exceptions\Warning as WarningException;
use PHPUnit\Interfaces\TestInterface;

use \PHPUnit_Framework_TestCase;
use \PHPUnit_Framework_TestResult;

/**
 * A warning.
 *
 * @since Class available since Release 2.0.0
 */
class WarningTestCase extends PHPUnit_Framework_TestCase implements TestInterface {
    /**
     * @var string
     */
    protected string $message = '';

    /**
     * @var bool
     */
    protected bool $backupGlobals = false;

    /**
     * @var bool
     */
    protected bool $backupStaticAttributes = false;

    /**
     * @var bool
     */
    protected bool $runTestInSeparateProcess = false;

    /**
     * @var bool
     */
    protected bool $useErrorHandler = false;

    /**
     * @param string $message
     */
    public function __construct(string $message = '') {
        $this->message = $message;
        parent::__construct('Warning');
    }

    /**
     * @throws PHPUnit_Exceptions_Exception
     */
    protected function runTest(): ?PHPUnit_Framework_TestResult {
        throw new WarningException($this->message);
    }

    /**
     * @return string
     *
     * @since Method available since Release 3.0.0
     */
    public function getMessage(): string {
        return $this->message;
    }

    /**
     * Returns a string representation of the test case.
     *
     * @return string
     *
     * @since Method available since Release 3.4.0
     */
    public function toString(): string {
        return 'Warning';
    }

}
