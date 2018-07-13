<?hh // strict

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace PHPUnit\Exceptions\Error;

use PHPUnit\Exceptions\Error\Base as BaseError;

/**
 * Wrapper for PHP notices.
 * You can disable notice-to-exception conversion by setting
 *
 * <code>
 * Notice::$enabled = false;
 * </code>
 *
 * @since Class available since Release 3.3.0
 */
class NoticeError extends BaseError {
    public static bool $enabled = true;
}
