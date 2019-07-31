<?hh // partial

namespace Zynga\PHPUnit\V2;

use Zynga\PHPUnit\V2\Exceptions\FailedToLoadFileException;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

/**
 * Utility methods to load PHP sourcefiles.
 *
 * @since Class available since Release 2.3.0
 */
class FileLoader {

  /**
   * Checks if a PHP sourcefile is readable.
   * The sourcefile is loaded through the load() method.
   *
   * @param string $filename
   *
   * @return string
   *
   * @throws PHPUnit_Framework_Exception
   */
  public static function checkAndLoad(string $filename): Vector<string> {

    $includePathFilename = stream_resolve_include_path($filename);

    if (!is_readable($includePathFilename)) {
      throw new FailedToLoadFileException(
        sprintf('Cannot open file "%s", unreadable', $filename),
      );
    }

    $newClasses = self::load($includePathFilename);

    return $newClasses;

  }

  /**
   * Loads a PHP sourcefile.
   *
   * @param string $filename
   *
   * @return Vector<string> newlyDeclaredClasses
   *
   * @since Method available since Release 3.0.0
   */
  public static function load(string $filename): Vector<string> {

    $beforeClasses = self::captureDeclaredClasses();

    include_once $filename;

    $afterClasses = self::captureDeclaredClasses();

    $diff = Vector {};

    $diff->addAll(
      array_diff($afterClasses->toArray(), $beforeClasses->toArray()),
    );

    return $diff;

  }

  public static function captureDeclaredClasses(): Vector<string> {

    $classes = Vector {};

    $classes->addAll(get_declared_classes());

    return $classes;

  }

}
