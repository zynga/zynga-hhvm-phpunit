<?php
/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use PHPUnit\Exceptions\AssertionFailedError;
use \PHPUnit_Framework_RiskyTestError;

// --
// JEO: Note, the comment here is misleading as it points to assertionfailederror,
// yet extends riskytesterror, in the future we should determine what breaks
// if we move it to not derive from risky test?
// --
/**
 * Extension to AssertionFailedError to mark the special
 * case of a test that does not execute the code it wants to cover.
 *
 * @since Class available since Release 5.2.0
 */
class PHPUnit_Framework_CoveredCodeNotExecutedException extends PHPUnit_Framework_RiskyTestError
{
}
