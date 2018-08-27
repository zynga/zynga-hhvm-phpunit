<?hh // strict

namespace SebastianBergmann\CodeCoverage\Report\Html\Renderer\Template;

class CodeRow {

  public static function render(
    int $lineNo,
    string $trClass,
    string $popover,
    string $lineStateCss,
    string $lineState,
    string $codeLine,
  ): string {

    return
      "<tr$trClass$popover>".
      "<td><span class=\"$lineStateCss\">$lineState</span></td>".
      "<td><div align=\"right\"><a name=\"$lineNo\"></a><a href=\"#$lineNo\">$lineNo</a></div></td>".
      "<td class=\"codeLine\">$codeLine</td>".
      "</tr>";

  }

}
