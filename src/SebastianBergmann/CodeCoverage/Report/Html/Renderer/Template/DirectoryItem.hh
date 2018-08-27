<?hh // strict

namespace SebastianBergmann\CodeCoverage\Report\Html\Renderer\Template;

class DirectoryItem {

  public static function render(
    string $icon,
    string $name,
    int $numClasses,
    string $classesLevel,
    string $testedClassesPercentAsString,
    string $classesBar,
    int $numMethods,
    string $methodsLevel,
    string $testedMethodsPercentAsString,
    string $methodsBar,
    string $linesNumber,
    string $linesLevel,
    string $linesExecutedPercentAsString,
    string $linesBar,
  ): string {
    return
      "<tr>".
      "<td class=\"$linesLevel\">$icon$name</td>".
      "<td class=\"$linesLevel big\">$classesBar</td>".
      "<td class=\"$classesLevel small\"><div align=\"right\">$testedClassesPercentAsString</div></td>".
      "<td class=\"$classesLevel small\"><div align=\"right\">$numClasses</div></td>".
      "<td class=\"$methodsLevel big\">$methodsBar</td>".
      "<td class=\"$methodsLevel small\"><div align=\"right\">$testedMethodsPercentAsString</div></td>".
      "<td class=\"$methodsLevel small\"><div align=\"right\">$numMethods</div></td>".
      "<td class=\"$linesLevel big\">$linesBar</td>".
      "<td class=\"$linesLevel small\"><div align=\"right\">$linesExecutedPercentAsString</div></td>".
      "<td class=\"$linesLevel small\"><div align=\"right\">$linesNumber</div></td>".
      "</tr>";
  }
}
