<?php
/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use Zynga\PHPUnit\V2\Exceptions\TestError\RiskyException;

/**
 * Extension to RiskyException to mark the special
 * case of a test that does not execute the code it wants to cover.
 *
 * @since Class available since Release 5.2.0
 */
class PHPUnit_Framework_CoveredCodeNotExecutedException extends RiskyException
{
}
