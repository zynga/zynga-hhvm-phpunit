<?hh // strict

namespace Zynga\Source;

use Zynga\Source\Cache\File as Zynga_Source_Cache_File;
use \Exception;

// JEO: As most of this functionality is disabled at this time turning off
// coverage until we can properly intrument and explore whats going on with the
// caching.
//
// @codeCoverageIgnoreStart
class Cache {

  private static Map<string, Zynga_Source_Cache_File> $_cache = Map {};

  public static function loadFile(string $fileName): Zynga_Source_Cache_File {

    throw new Exception('This is deprecated');

    if (self::$_cache->containsKey($fileName)) {
      return self::$_cache[$fileName];
    }

    // --
    // JEO: There is a ton of pre-filtering before this class gets hit,
    // disabling this check for now.
    // --
    /*
     if ( is_file($fileName) !== true ) {
     return null;
     }
     */

    $file = new Zynga_Source_Cache_File($fileName);
    $file->load();

    self::$_cache[$fileName] = $file;

    return $file;

  }

  public static function getSource(string $fileName): string {

    $file = self::loadFile($fileName);

    return $file->source;

  }

  public static function getTokens(string $fileName): mixed {

    $file = self::loadFile($fileName);

    return $file->tokens;

  }

  public static function getCodeCoverageRaw(string $fileName): mixed {

    $file = self::loadFile($fileName);

    if ($file instanceof Zynga_Source_Cache_File &&
        $file->codeCoverageRaw !== null) {
      return $file->codeCoverageRaw;
    }

    return null;

  }

  public static function setCodeCoverageRaw(
    string $fileName,
    mixed $raw,
  ): bool {

    $file = self::loadFile($fileName);

    if ($file instanceof Zynga_Source_Cache_File) {

      $file->codeCoverageRaw = $raw;

      return true;

    }

    return false;

  }

  public static function getIgnoredLines(string $fileName): mixed {

    $file = self::loadFile($fileName);

    if ($file instanceof Zynga_Source_Cache_File &&
        $file->ignoredLines !== null) {
      return $file->ignoredLines;
    }

    return null;

  }

  public static function setIgnoredLines(
    string $fileName,
    mixed $ignoredLines,
  ): bool {
    $file = self::loadFile($fileName);

    if ($file instanceof Zynga_Source_Cache_File) {

      $file->ignoredLines = $ignoredLines;

      return true;

    }

    return false;
  }

  public static function getFileCoverageObject(string $fileName): mixed {
    return null;
  }

  public static function setFileCoverageObject(
    string $fileName,
    mixed $obj,
  ): bool {
    return false;
  }

}

// @codeCoverageIgnoreEnd
