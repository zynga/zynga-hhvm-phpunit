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

use PHPUnit\Exceptions\AssertionFailedError;

/**
 * Creates a synthetic failed assertion.
 *
 * @since Class available since Release 3.5.0
 */
class SyntheticError extends AssertionFailedError {
    /**
     * The synthetic file.
     *
     * @var string
     */
    protected string $syntheticFile = '';

    /**
     * The synthetic line number.
     *
     * @var int
     */
    protected int $syntheticLine = 0;

    /**
     * The synthetic trace.
     *
     * @var array
     */
    protected Vector<mixed> $syntheticTrace = Vector {};

    /**
     * Constructor.
     *
     * @param string $message
     * @param int    $code
     * @param string $file
     * @param int    $line
     * @param array  $trace
     */
    public function __construct(string $message, int $code, string $file, int $line, Vector<mixed> $trace) {
        parent::__construct($message, $code);

        $this->syntheticFile  = $file;
        $this->syntheticLine  = $line;
        $this->syntheticTrace = $trace;
    }

    /**
     * @return string
     */
    public function getSyntheticFile(): string {
        return $this->syntheticFile;
    }

    /**
     * @return int
     */
    public function getSyntheticLine(): int {
        return $this->syntheticLine;
    }

    /**
     * @return array
     */
    public function getSyntheticTrace(): Vector<mixed> {
        return $this->syntheticTrace;
    }
    
}
