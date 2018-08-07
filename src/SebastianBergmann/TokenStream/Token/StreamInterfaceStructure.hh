<?hh // strict

namespace SebastianBergmann\TokenStream\Token;

use SebastianBergmann\TokenStream\Token\StreamMethodStructure;

class StreamInterfaceStructure {
  public Map<string, StreamMethodStructure> $methods = Map {};
  public mixed $parent = false;
  public mixed $interfaces = false;
  public string $keywords = '';
  public string $docblock = '';
  public int $startLine = -1;
  public int $endLine = -1;
  public Map<string, string> $package = Map {};
  public string $file = '';
}
