<?hh // strict

namespace Zynga\Source\Cache;

use \Memcached;
use Zynga\Source\Cache\LocalMemcache as Zynga_Source_Cache_LocalMemcache;

class File {

  public string $fileName;
  public string $source;
  public mixed $tokens;
  public mixed $codeCoverageRaw;
  public mixed $ignoredLines;

  public function __construct(string $fileName) {

    $this->fileName = $fileName;
    $this->source = '';
    $this->tokens = null;
    $this->codeCoverageRaw = null;
    $this->ignoredLines = null;

  }

  public function load(): bool {
    $source = file_get_contents($this->fileName);
    // given source code store it and the tokens associated to it on the object.
    $this->source = $source;
    $this->tokens = token_get_all($source);
    return true;
  }

}
