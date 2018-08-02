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

/**
 * A caching factory for token stream objects.
 */
class CachingFactory {
  /**
   * @var array
   */
  protected static Map<string, Stream> $cache = Map {};

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

    $stream = new Stream($filename);

    self::$cache->set($filename, $stream);

    return $stream;

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
