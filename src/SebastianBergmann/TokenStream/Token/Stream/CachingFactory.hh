<?hh // strict

namespace SebastianBergmann\TokenStream\Token\Stream;

use SebastianBergmann\TokenStream\Token\Stream;

/*
 * This file is part of php-token-stream.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use \Exception;

/**
 * A caching factory for token stream objects.
 */
class CachingFactory {

  protected static int $streamId = 0;

  protected static Map<string, Stream> $cache = Map {};
  protected static Map<int, string> $idToNameCache = Map {};

  /**
   * @param string $filename
   *
   * @return Stream
   */
  public static function get(string $filename): Stream {

    $cachedTokenStream = self::$cache->get($filename);

    if ($cachedTokenStream instanceof Stream) {
      return $cachedTokenStream;
    }

    self::$streamId++;

    $stream = new Stream($filename, self::$streamId);

    self::$cache->set($filename, $stream);
    self::$idToNameCache->set(self::$streamId, $filename);

    return $stream;

  }

  public static function getByStreamId(int $streamId): Stream {

    $filename = self::$idToNameCache->get($streamId);

    if (is_string($filename)) {
      $cachedStream = self::$cache->get($filename);

      if ($cachedStream instanceof Stream) {
        return $cachedStream;
      }

    }

    throw new Exception('Failed to lookup streamId='.$streamId);

  }

  /**
   * @param string $filename
   */
  public static function clear(?string $filename = null): void {
    /*
     if (is_string($filename)) {
     unset(self::$cache[$filename]);
     } else {
     self::$cache = [];
     }
     */
  }
}
