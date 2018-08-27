<?hh // strict

namespace SebastianBergmann\CodeCoverage\Report\Html\Renderer\Template;

use
  SebastianBergmann\CodeCoverage\Report\Html\Renderer\CommonTemplateVariables
;

class CodeFileItem {

  public static function render(
    string $name,
    string $classes_number,
    string $classes_level,
    string $classes_tested_percent,
    string $classes_bar,
    string $methods_number,
    string $methods_level,
    string $methods_tested_percent,
    string $methods_bar,
    string $lines_number,
    string $lines_level,
    string $lines_tested_percent,
    string $lines_bar,
    string $ccn,
    string $crap,
  ): string {
    return
      "<tr>".
      "<td class=\"$classes_level\">$name</td>".
      "<td class=\"$classes_level big\">$classes_bar</td>".
      "<td class=\"$classes_level small\"><div align=\"right\">$classes_tested_percent</div></td>".
      "<td class=\"$classes_level small\"><div align=\"right\">$classes_number</div></td>".
      "<td class=\"$methods_level big\">$methods_bar</td>".
      "<td class=\"$methods_level small\"><div align=\"right\">$methods_tested_percent</div></td>".
      "<td class=\"$methods_level small\"><div align=\"right\">$methods_number</div></td>".
      "<td class=\"$methods_level small\">$crap</td>".
      "<td class=\"$methods_level small\">$ccn</td>".
      "<td class=\"$lines_level big\">$lines_bar</td>".
      "<td class=\"$lines_level small\"><div align=\"right\">$lines_tested_percent</div></td>".
      "<td class=\"$lines_level small\"><div align=\"right\">$lines_number</div></td>".
      "</tr>";
  }

}
