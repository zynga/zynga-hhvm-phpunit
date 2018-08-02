<?hh // strict

namespace SebastianBergmann\TokenStream\Token;

class StreamFunctionStructure {
  public string $docblock = '';
  public string $keywords = '';
  public string $visibility = '';
  public string $signature = '';
  public int $startLine = 0;
  public int $endLine = 0;
  public int $ccn = 0;
  public string $file = '';
}
