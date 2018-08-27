<?hh // strict

namespace SebastianBergmann\CodeCoverage\Report\Html\Renderer\Template;

class CoverageBar {

  public static function render(string $level, string $percent): string {
    return
      "<div class=\"progress\">".
      "<div class=\"progress-bar progress-bar-$level\" role=\"progressbar\" aria-valuenow=\"$percent\" aria-valuemin=\"0\" aria-valuemax=\"100\" style=\"width: $percent%\">".
      "<span class=\"sr-only\">$percent% covered ($level)</span>".
      "</div>".
      "</div>";
  }

}
