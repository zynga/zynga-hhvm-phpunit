<?php
/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use PHPUnit\Exceptions\Exception as PHPUnit_Exceptions_Exception;

/**
 * Thrown when an assertion failed.
 *
 * @since Class available since Release 2.0.0
 */
class PHPUnit_Framework_AssertionFailedError extends PHPUnit_Exceptions_Exception implements PHPUnit_Framework_SelfDescribing
{
    /**
     * Wrapper for getMessage() which is declared as final.
     *
     * @return string
     */
    public function toString()
    {
        return $this->getMessage();
    }
}
