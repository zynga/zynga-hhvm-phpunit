<?hh // strict

namespace SebastianBergmann\TokenStream\Token;

class StreamTraitStructure {
  public Map<string, StreamFunctionStructure> $methods = Map {};
  public mixed $parent = false;
  public mixed $interfaces = false;
  public string $keywords = '';
  public string $docblock = '';
  public int $startLine = -1;
  public int $endLine = -1;
  public Map<string, string> $package = Map {};
  public string $file = '';
}
