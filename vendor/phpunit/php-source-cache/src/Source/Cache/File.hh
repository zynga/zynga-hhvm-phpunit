<?hh // strict

namespace Zynga\Source\Cache;

use \Memcached;
use Zynga\Source\Cache\LocalMemcache as Zynga_Source_Cache_LocalMemcache;

// @codeCoverageIgnoreStart
class File {

  const int CACHE_TTL = 86400;

  private string $_mcKey;

  public string $fileName;
  public string $source;
  public mixed $tokens;
  public mixed $codeCoverageRaw;
  public mixed $ignoredLines;
  public int $lastModified;

  public function __construct(string $fileName) {

    $this->_mcKey = '';

    $this->fileName = $fileName;
    $this->source = '';
    $this->tokens = null;
    $this->codeCoverageRaw = null;
    $this->ignoredLines = null;
    $this->lastModified = 0;

  }

  public function getMemcached(): Memcached {
    return Zynga_Source_Cache_LocalMemcache::getMemcached();
  }

  public function load(): bool {

    $this->loadFromMemcache();

    if ( $this->isFileDataStale() !== true ) {
      return true;
    }

    // purge any remnants from memcache.
    $this->purge();

    // fetch the last modified from the filesystem
    $this->updateLastModified();

    // load up the source for the file.
    $this->loadSource();

    // save off the data to memcache
    $this->saveToMemcache();

    return true;

  }

  public function loadSource(): bool {
    $source = file_get_contents($this->fileName);
    // given source code store it and the tokens associated to it on the object.
    $this->source = $source;
    $this->tokens = token_get_all($source);
    return true;
  }

  public function updateLastModified(): bool {
    $this->lastModified = filemtime($this->fileName);
    return true;
  }

  public function loadFromMemcache(): bool {

    // --
    //
    // JEO: TODO, it's late on 2017/04/25, had to turn off the fetching from
    // mc, I think theres a issue where if the unit test changes, but the
    // code didn't we don't actually reparse correctly.
    //
    // Need coverage to work for tomorrows presentation.
    //
    // --
    return false;

    /*
    try {


      $key = $this->createKey();

      $mc = $this->getMemcached();

      $data = $mc->get($key);

      // var_dump($data);
      if ( $data instanceof Zynga_Source_Cache_File && $data->lastModified > 0 ) {

        $this->lastModified = $data->lastModified;
        $this->source = $data->source;
        $this->tokens = $data->tokens;
        $this->codeCoverageRaw = $data->codeCoverageRaw;
        $this->ignoredLines = $data->ignoredLines;

        //var_dump($this);
        //exit();

        return true;

      }

      return false;

    } catch ( Exception $e ) {
      return false;
    }
    */

  }

  public function saveToMemcache(): bool {

    // --
    //
    // JEO: TODO, it's late on 2017/04/25, had to turn off the fetching from
    // mc, I think theres a issue where if the unit test changes, but the
    // code didn't we don't actually reparse correctly.
    //
    // Need coverage to work for tomorrows presentation.
    //
    // --
    return true;

    /*
    try {


      $key = $this->createKey();

      $ttl = self::CACHE_TTL + time();

      $mc = $this->getMemcached();

      // echo "saving fileName=" . $this->fileName . " key=" . $key . "\n";
      $mc->set($key, $this, $ttl);

      return true;

    } catch ( Exception $e ) {
      return false;
    }
    */

  }

  public function purge(): bool {

    // caching is disabled for now.
    return true;

    /*
    try {

      $key = $this->createKey();

      $mc = $this->getMemcached();

      $mc->delete($key);

      return true;

    } catch ( Exception $e ) {
      return false;
    }
    */

  }

  public function isFileDataStale(): bool {

    // caching is disabled for now.
    return true;

    /*
    $lastModified = filemtime($this->fileName);

    if ( $this->lastModified != $lastModified ) {
      return true;
    }

    return false;
    */

  }


  public function createKey(): string {

    // caching is disabled for now.
    return '';

    /*
    if ( $this->_mcKey !== '' ) {
      return $this->_mcKey;
    }

    $version = 4;
    $key = 'zsc|jeo:' . $version . ':' . md5($this->fileName);

    $this->_mcKey = $key;

    // var_dump($this->_mcKey);

    return $this->_mcKey;
    */

  }

}
// @codeCoverageIgnoreEnd
