<?hh // strict

namespace SebastianBergmann\CodeCoverage\Report\Html\Renderer\Template;

class CodeLine {

  public static function render(string $code_class, string $code): string {
    return "<span class=\"$code_class\">$code</span>";
  }

}
