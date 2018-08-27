<?hh // strict

namespace SebastianBergmann\CodeCoverage\Report\Html\Renderer\Template;

class CodeTokens {

  public static function render(int $lineNo, string $tokensForLine): string {
    return
      "<tr>".
      "<td colspan=\"2\"><div align=\"right\"><a name=\"$lineNo\"></a><a href=\"#$lineNo\">$lineNo</a></div></td>".
      "<td class=\"codeLine\"><span class=\"comment\">Tokens: $tokensForLine</span></td>".
      "</tr>";
  }

}
