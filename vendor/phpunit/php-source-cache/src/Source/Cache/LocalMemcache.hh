<?hh // strict

namespace Zynga\Source\Cache;

use \Memcached;
use \Exception;

// @codeCoverageIgnoreStart
class LocalMemcache {

  private static ?Memcached $_mc;

  public static function getMemcached(): Memcached {
    try {

      if ( self::$_mc !== null ) {
        return self::$_mc;
      }

      $mc = new Memcached();
      $mc->addServer('localhost', 11211);

      self::$_mc = $mc;

      return self::$_mc;
    } catch ( Exception $e ) {
      throw $e;
    }
  }

}
// @codeCoverageIgnoreEnd
