<?hh // strict

/*
 * This file is part of the php-code-coverage package.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace SebastianBergmann\CodeCoverage\Report\Html;

use SebastianBergmann\CodeCoverage\Node\AbstractNode;
use SebastianBergmann\CodeCoverage\Node\File as FileNode;
use SebastianBergmann\CodeCoverage\Node\Directory as DirectoryNode;
use SebastianBergmann\Environment\Runtime;
use SebastianBergmann\TextTemplate\Template;
use SebastianBergmann\TextTemplate\TemplateFactory;
use Zynga\PHPUnit\V2\Version;

/**
 * Base class for node renderers.
 */
abstract class Renderer {
  /**
   * @var string
   */
  protected string $templatePath;

  /**
   * @var string
   */
  protected string $generator;

  /**
   * @var string
   */
  protected string $date;

  /**
   * @var int
   */
  protected int $lowUpperBound;

  /**
   * @var int
   */
  protected int $highLowerBound;

  /**
   * @var string
   */
  protected string $version;

  /**
   * Constructor.
   *
   * @param string $templatePath
   * @param string $generator
   * @param string $date
   * @param int    $lowUpperBound
   * @param int    $highLowerBound
   */
  public function __construct(
    string $templatePath,
    string $generator,
    string $date,
    int $lowUpperBound,
    int $highLowerBound,
  ) {
    $this->templatePath = $templatePath;
    $this->generator = $generator;
    $this->date = $date;
    $this->lowUpperBound = $lowUpperBound;
    $this->highLowerBound = $highLowerBound;
    $this->version = Version::get();
  }

  /**
   * @param \Text_Template $template
   * @param array          $data
   *
   * @return string
   */
  protected function renderItemTemplate(
    Template $template,
    Map<string, mixed> $data,
  ): string {
    $numSeparator = '&nbsp;/&nbsp;';

    $numClasses = intval($data->get('numClasses'));
    $numTestedClasses = intval($data->get('numTestedClasses'));

    if ($numClasses > 0) {
      $classesLevel =
        $this->getColorLevel(floatval($data['testedClassesPercent']));
      $classesNumber = $numTestedClasses.$numSeparator.$numClasses;
      $classesBar =
        $this->getCoverageBar(floatval($data['testedClassesPercent']));
    } else {
      $classesLevel = 'success';
      $classesNumber = '0'.$numSeparator.'0';
      $classesBar = $this->getCoverageBar(100.00);
    }

    $numMethods = intval($data->get('numMethods'));
    $numTestedMethods = intval($data->get('numTestedMethods'));

    if ($numMethods > 0) {
      $methodsLevel =
        $this->getColorLevel(floatval($data['testedMethodsPercent']));
      $methodsNumber = $numTestedMethods.$numSeparator.$numMethods;
      $methodsBar =
        $this->getCoverageBar(floatval($data['testedMethodsPercent']));
    } else {
      $methodsLevel = 'success';
      $methodsNumber = '0'.$numSeparator.'0';
      $methodsBar = $this->getCoverageBar(100.00);
      $data['testedMethodsPercentAsString'] = '100.00%';
    }

    $numExecutableLines = intval($data->get('numExecutableLines'));
    $numExecutedLines = intval($data->get('numExecutedLines'));

    if ($numExecutableLines > 0) {
      $linesLevel =
        $this->getColorLevel(floatval($data['linesExecutedPercent']));
      $linesNumber = $numExecutedLines.$numSeparator.$numExecutableLines;
      $linesBar =
        $this->getCoverageBar(floatval($data['linesExecutedPercent']));
    } else {
      $linesLevel = 'success';
      $linesNumber = '0'.$numSeparator.'0';
      $linesBar = $this->getCoverageBar(100.00);
      $data['linesExecutedPercentAsString'] = '100.00%';
    }

    $templateVariables = Map {
      'icon' => $data->containsKey('icon') ? $data['icon'] : '',
      'crap' => $data->containsKey('crap') ? $data['crap'] : '',
      'name' => $data['name'],
      'lines_bar' => $linesBar,
      'lines_executed_percent' => $data['linesExecutedPercentAsString'],
      'lines_level' => $linesLevel,
      'lines_number' => $linesNumber,
      'methods_bar' => $methodsBar,
      'methods_tested_percent' => $data['testedMethodsPercentAsString'],
      'methods_level' => $methodsLevel,
      'methods_number' => $methodsNumber,
      'classes_bar' => $classesBar,
      'classes_tested_percent' =>
        $data->containsKey('testedClassesPercentAsString')
          ? $data['testedClassesPercentAsString']
          : '',
      'classes_level' => $classesLevel,
      'classes_number' => $classesNumber,
    };

    return $template->render($templateVariables);
  }

  /**
   * @param \Text_Template $template
   * @param AbstractNode   $node
   */
  protected function getCommonTemplateVariables(
    AbstractNode $node,
  ): Map<string, mixed> {
    $templateVars = Map {
      'id' => $node->getId(),
      'full_path' => $node->getPath(),
      'path_to_root' => $this->getPathToRoot($node),
      'breadcrumbs' => $this->getBreadcrumbs($node),
      'date' => $this->date,
      'version' => $this->version,
      'runtime' => $this->getRuntimeString(),
      'generator' => $this->generator,
      'low_upper_bound' => $this->lowUpperBound,
      'high_lower_bound' => $this->highLowerBound,
    };
    return $templateVars;
  }

  protected function getBreadcrumbs(AbstractNode $node): string {
    $breadcrumbs = '';
    $path = $node->getPathAsArray();
    $pathToRoot = [];
    $max = count($path);

    if ($node instanceof FileNode) {
      $max--;
    }

    for ($i = 0; $i < $max; $i++) {
      $pathToRoot[] = str_repeat('../', $i);
    }

    foreach ($path as $step) {
      if ($step !== $node) {
        $breadcrumbs .=
          $this->getInactiveBreadcrumb($step, array_pop($pathToRoot));
      } else {
        $breadcrumbs .= $this->getActiveBreadcrumb($step);
      }
    }

    return $breadcrumbs;
  }

  protected function getActiveBreadcrumb(AbstractNode $node): string {
    $buffer =
      sprintf('        <li class="active">%s</li>'."\n", $node->getName());

    if ($node instanceof DirectoryNode) {
      $buffer .=
        '        <li>(<a href="dashboard.html">Dashboard</a>)</li>'."\n";
    }

    return $buffer;
  }

  protected function getInactiveBreadcrumb(
    AbstractNode $node,
    string $pathToRoot,
  ): string {
    return sprintf(
      '        <li><a href="%sindex.html">%s</a></li>'."\n",
      $pathToRoot,
      $node->getName(),
    );
  }

  protected function getPathToRoot(AbstractNode $node): string {
    return '/';
    /*
     $id = $node->getId();
     $depth = substr_count($id, '/');
     if ($id != 'index' && $node instanceof DirectoryNode) {
     $depth++;
     }
     return str_repeat('../', $depth);
     */
  }

  protected function getCoverageBar(float $percent): string {
    $level = $this->getColorLevel($percent);

    $template = TemplateFactory::get(
      $this->templatePath.'coverage_bar.html',
      '{{',
      '}}',
    );

    $templateVariables = Map {
      'level' => $level,
      'percent' => sprintf('%.2F', $percent),
    };

    return $template->render($templateVariables);

  }

  /**
   * @param int $percent
   *
   * @return string
   */
  protected function getColorLevel(float $percent): string {
    if ($percent <= $this->lowUpperBound) {
      return 'danger';
    } else if ($percent > $this->lowUpperBound &&
               $percent < $this->highLowerBound) {
      return 'warning';
    } else {
      return 'success';
    }
  }

  /**
   * @return string
   */
  private function getRuntimeString(): string {
    $runtime = new Runtime();

    $buffer = sprintf(
      '<a href="%s" target="_top">%s %s</a>',
      $runtime->getVendorUrl(),
      $runtime->getName(),
      $runtime->getVersion(),
    );

    if ($runtime->hasXdebug()) {
      $buffer .= sprintf(
        ' with <a href="https://xdebug.org/">Xdebug %s</a>',
        phpversion('xdebug'),
      );
    }

    return $buffer;
  }
}
