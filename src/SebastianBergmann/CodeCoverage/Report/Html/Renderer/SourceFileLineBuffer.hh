<?hh // strict

namespace SebastianBergmann\CodeCoverage\Report\Html\Renderer;

// --
// JEO: Why are we utilizing an array<string> here, because it's flat out more
//   performant than a Vector<string> or Map<int, string> variant.
// --
class SourceFileLineBuffer {
  private array<string> $_lines;

  public function __construct() {
    $this->_lines = array();
  }

  public function set(int $lineNo, string $value): void {
    $this->_lines[$lineNo] = $value;
  }

  public function appendTo(int $lineNo, string $value): void {
    $this->_lines[$lineNo] .= $value;
  }

  public function get(): array<string> {
    return $this->_lines;
  }

}
