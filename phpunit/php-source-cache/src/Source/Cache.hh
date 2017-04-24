<?hh // strict

class Zynga_Source_Cache_File {

  const int CACHE_TTL = 86400;

  private string $_mcKey;

  public string $fileName;
  public ?string $source;
  public mixed $tokens;
  public mixed $codeCoverageRaw;
  public mixed $ignoredLines;
  public int $lastModified;

  public function __construct(string $fileName) {

    $this->_mcKey = '';

    $this->fileName = $fileName;
    $this->source = null;
    $this->tokens = null;
    $this->codeCoverageRaw = null;
    $this->ignoredLines = null;
    $this->lastModified = 0;

  }

  public function getMemcached(): Memcached {
    return Zynga_Source_Cache_LocalMemcache::getMemcached();
  }

  public function loadFromMemcache(): bool {

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

  }

  public function saveToMemcache(): bool {

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

  }

  public function purge(): bool {

    try {

      $key = $this->createKey();

      $mc = $this->getMemcached();

      $mc->delete($key);

      return true;

    } catch ( Exception $e ) {
      return false;
    }

  }

  public function isFileDataStale(): bool {

    $lastModified = filemtime($this->fileName);

    if ( $this->lastModified != $lastModified ) {
      return true;
    }

    return false;
  }


  public function createKey(): string {

    if ( $this->_mcKey !== '' ) {
      return $this->_mcKey;
    }

    $version = 4;
    $key = 'zsc|jeo:' . $version . ':' . md5($this->fileName);

    $this->_mcKey = $key;

    // var_dump($this->_mcKey);

    return $this->_mcKey;

  }

}

class Zynga_Source_Cache {

  private static Map<string, Zynga_Source_Cache_File> $_cache = Map {};

  public static function loadFile(string $fileName): ?Zynga_Source_Cache_File {

    if ( self::$_cache->containsKey($fileName) ) {
      return self::$_cache[$fileName];
    }

    if ( is_file($fileName) !== true ) {
      return null;
    }

    $file = new Zynga_Source_Cache_File($fileName);
    $file->loadFromMemcache();

    if ( $file->isFileDataStale() == true ) {
      $file->purge();
      $file->lastModified = filemtime($file->fileName);
    }

    $file->saveToMemcache();

    self::$_cache[$fileName] = $file;

    return $file;

  }

  public static function getSource(string $fileName): string {

    $file = self::loadFile($fileName);

    if ( $file instanceof Zynga_Source_Cache_File && $file->source !== null ) {
      return $file->source;
    }

    $source = file_get_contents($fileName);

    if ( $file instanceof Zynga_Source_Cache_File && is_string($source) ) {

      $file->source = $source;
      $file->saveToMemcache();

      if ( is_string($file->source) ) {
        return $file->source;
      }

    }

    return $source;

  }

  public static function getTokens(string $fileName): mixed {

    $file = self::loadFile($fileName);

    if ( $file instanceof Zynga_Source_Cache_File && $file->tokens !== null ) {
      return $file->tokens;
    }

    $source = self::getSource($fileName);

    if ( $file instanceof Zynga_Source_Cache_File && is_string($source) ) {

      $file->tokens = token_get_all($source);
      $file->saveToMemcache();

      return $file->tokens;

    }

    return null;

  }

  public static function getCodeCoverageRaw(string $fileName): mixed {

    $file = self::loadFile($fileName);

    if ( $file instanceof Zynga_Source_Cache_File && $file->codeCoverageRaw !== null ) {
      return $file->codeCoverageRaw;
    }

    return null;

  }

  public static function setCodeCoverageRaw(string $fileName, mixed $raw): bool {

    $file = self::loadFile($fileName);

    if ( $file instanceof Zynga_Source_Cache_File ) {

      $file->codeCoverageRaw = $raw;
      $file->saveToMemcache();

      return true;

    }

    return false;

  }

  public static function getIgnoredLines(string $fileName): mixed {

    $file = self::loadFile($fileName);

    if ( $file instanceof Zynga_Source_Cache_File && $file->ignoredLines !== null ) {
      return $file->ignoredLines;
    }

    return null;

  }

  public static function setIgnoredLines(string $fileName, mixed $ignoredLines): bool {
    $file = self::loadFile($fileName);

    if ( $file instanceof Zynga_Source_Cache_File ) {

      $file->ignoredLines = $ignoredLines;
      $file->saveToMemcache();

      return true;

    }

    return false;
  }

  public static function getFileCoverageObject(string $fileName): mixed {
    return null;
  }

  public static function setFileCoverageObject(string $fileName, mixed $obj): bool {
    return false;
  }

}

class Zynga_Source_Cache_LocalMemcache {

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
