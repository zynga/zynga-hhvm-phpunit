<?hh // strict

/*
 * This file is part of the php-code-coverage package.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace SebastianBergmann\CodeCoverage\Report\Html\Renderer;

use SebastianBergmann\CodeCoverage\Node\AbstractNode as Node;
use SebastianBergmann\CodeCoverage\Node\Directory as DirectoryNode;
use SebastianBergmann\CodeCoverage\Report\Html\Renderer;
use SebastianBergmann\TextTemplate\TemplateFactory;
use SebastianBergmann\TextTempalte\Template as Text_Template;
use
  SebastianBergmann\CodeCoverage\Report\Html\Renderer\Template\DirectoryItem
;
/**
 * Renders a directory node.
 */
class Directory extends Renderer {

  /**
   * @param DirectoryNode $node
   * @param string        $file
   */
  public function render(DirectoryNode $node, string $file): void {

    $template =
      TemplateFactory::get($this->templatePath.'directory.html', '{{', '}}');

    $templateVariables = $this->getCommonTemplateVariables($node);
    $templateVariableMap = $templateVariables->asMap();

    $items = $this->renderItem($node, true);

    foreach ($node->getDirectories() as $item) {
      $items .= $this->renderItem($item);
    }

    foreach ($node->getFiles() as $item) {
      $items .= $this->renderItem($item);
    }

    $templateVariableMap->set('id', $node->getId());
    $templateVariableMap->set('items', $items);

    $template->renderTo($file, $templateVariableMap);

  }

  /**
   * @param Node $node
   * @param bool $total
   *
   * @return string
   */
  protected function renderItem(Node $node, bool $total = false): string {

    $name = '';
    $icon = '';
    $crap = '';

    if ($total === true) {
      $name = 'Total';
    } else {
      if ($node instanceof DirectoryNode) {
        $name = sprintf(
          '<a href="%s/index.html">%s</a>',
          $node->getId(),
          $node->getId(),
        );

        $icon = '<span class="glyphicon glyphicon-folder-open"></span> ';
        $crap = $node->getCrapAsString();
      } else {
        $name =
          sprintf('<a href="%s.html">%s</a>', $node->getId(), $node->getId());

        $icon = '<span class="glyphicon glyphicon-file"></span> ';
      }
    }

    $numSeparator = '&nbsp;/&nbsp;';

    $numClasses = $node->getNumClassesAndTraits();
    $numTestedClasses = $node->getNumTestedClassesAndTraits();
    $numMethods = $node->getNumMethods();
    $numTestedMethods = $node->getNumTestedMethods();
    $linesExecutedPercent = $node->getLineExecutedPercent();
    $linesExecutedPercentAsString = $node->getLineExecutedPercentAsString();
    $numExecutedLines = $node->getNumExecutedLines();
    $numExecutableLines = $node->getNumExecutableLines();
    $testedMethodsPercent = $node->getTestedMethodsPercent();
    $testedMethodsPercentAsString = $node->getTestedMethodsPercentAsString();
    $testedClassesPercent = $node->getTestedClassesAndTraitsPercent();
    $testedClassesPercentAsString =
      $node->getTestedClassesAndTraitsPercentAsString();

    if ($numClasses > 0) {
      $classesLevel = $this->getColorLevel($testedClassesPercent);
      $classesNumber = $numTestedClasses.$numSeparator.$numClasses;
      $classesBar = $this->getCoverageBar($testedClassesPercent);
    } else {
      $classesLevel = 'success';
      $classesNumber = '0'.$numSeparator.'0';
      $classesBar = $this->getCoverageBar(100.00);
    }

    if ($numMethods > 0) {
      $methodsLevel = $this->getColorLevel($testedMethodsPercent);
      $methodsNumber = $numTestedMethods.$numSeparator.$numMethods;
      $methodsBar = $this->getCoverageBar($testedMethodsPercent);
    } else {
      $methodsLevel = 'success';
      $methodsNumber = '0'.$numSeparator.'0';
      $methodsBar = $this->getCoverageBar(100.00);
      $testedMethodsPercentAsString = '100.00%';
    }

    if ($numExecutableLines > 0) {
      $linesLevel = $this->getColorLevel($linesExecutedPercent);
      $linesNumber = $numExecutedLines.$numSeparator.$numExecutableLines;
      $linesBar = $this->getCoverageBar($linesExecutedPercent);
    } else {
      $linesLevel = 'success';
      $linesNumber = '0'.$numSeparator.'0';
      $linesBar = $this->getCoverageBar(100.00);
      $linesExecutedPercentAsString = '100.00%';
    }

    return DirectoryItem::render(
      $icon,
      $name,
      $numClasses,
      $classesLevel,
      $testedClassesPercentAsString,
      $classesBar,
      $numMethods,
      $methodsLevel,
      $testedMethodsPercentAsString,
      $methodsBar,
      $linesNumber,
      $linesLevel,
      $linesExecutedPercentAsString,
      $linesBar,
    );

  }

}
