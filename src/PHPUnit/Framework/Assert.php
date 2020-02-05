<?hh // strict

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use Zynga\PHPUnit\V2\Assertions;

// --
// We are in the process of porting all of these assertion bits into hacklang
// strict code.
//
// When we complete all the porting, we will be attempting removal of the base
//  level class of PHPUnit_Framework_Assert
//
// --
/**
 * A set of assert methods.
 *
 * @since Class available since Release 2.0.0
 */
abstract class PHPUnit_Framework_Assert extends Assertions {}
