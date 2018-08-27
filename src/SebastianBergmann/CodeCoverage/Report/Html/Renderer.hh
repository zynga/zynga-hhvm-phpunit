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
use
  SebastianBergmann\CodeCoverage\Report\Html\Renderer\CommonTemplateVariables
;

use SebastianBergmann\Environment\Runtime;
use SebastianBergmann\TextTemplate\Template;
use SebastianBergmann\CodeCoverage\Report\Html\Renderer\Template\CoverageBar;
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

  protected DirectoryNode $root;

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
    DirectoryNode $root,
  ) {
    $this->templatePath = $templatePath;
    $this->generator = $generator;
    $this->date = $date;
    $this->lowUpperBound = $lowUpperBound;
    $this->highLowerBound = $highLowerBound;
    $this->version = Version::get();
    $this->root = $root;
  }

  /**
   * @param \Text_Template $template
   * @param array          $data
   *
   * @return string
   */

  /**
   * @param \Text_Template $template
   * @param AbstractNode   $node
   */
  protected function getCommonTemplateVariables(
    AbstractNode $node,
  ): CommonTemplateVariables {

    $commonTemplateVariables = new CommonTemplateVariables();
    $commonTemplateVariables->id = strval($node->getId());
    $commonTemplateVariables->full_path = strval($node->getPath());
    $commonTemplateVariables->path_to_root = $this->getPathToRoot($node);
    $commonTemplateVariables->breadcrumbs = $this->getBreadcrumbs($node);
    $commonTemplateVariables->date = $this->date;
    $commonTemplateVariables->version = $this->version;
    $commonTemplateVariables->runtime = $this->getRuntimeString();
    $commonTemplateVariables->generator = $this->generator;
    $commonTemplateVariables->low_upper_bound = strval($this->lowUpperBound);
    $commonTemplateVariables->high_lower_bound =
      strval($this->highLowerBound);

    return $commonTemplateVariables;
  }

  protected function getBreadcrumbs(AbstractNode $node): string {

    $longFormPath = $node->getPath();

    $dirName = '';
    $fileName = '';

    $rootPath = $this->root->getPath();

    if ($node instanceof FileNode) {
      // this is a file therefor the path needs to be striped of the filename.
      $dirName = dirname($longFormPath);
      $fileName =
        str_replace($dirName.DIRECTORY_SEPARATOR, '', $longFormPath);
    } else {
      $dirName = $longFormPath;
    }

    $dirName = str_replace($rootPath, '', $dirName);

    $breadcrumbs = '';

    // okay let's make some directory path crumbs
    $dirCrumbs = explode("/", $dirName);
    $dirCrumbCount = count($dirCrumbs);
    $lastCrumb = '';
    if ($dirCrumbCount > 1) {
      $lastCrumb = $dirCrumbs[$dirCrumbCount - 1];
    }

    if ($dirCrumbCount > 0) {

      $offset = 0;
      foreach ($dirCrumbs as $dirCrumb) {

        $shortPath = implode("/", array_slice($dirCrumbs, 0, $offset))."/";

        // ensure that all our paths have a starting slash and ending slash.
        if (preg_match('/^\//', $shortPath) != true) {
          $shortPath = '/'.$shortPath;
        }
        if (preg_match('/\/$/', $shortPath) != true) {
          $shortPath = $shortPath.'/';
        }

        if ($fileName === '' && $lastCrumb == $dirCrumb) {
          $breadcrumbs .= $this->getActiveBreadcrumb($shortPath, $dirCrumb);
          $breadcrumbs .= '<li>(<a href="dashboard.html">Dashboard</a>)</li>';
        } else {
          $breadcrumbs .= $this->getInactiveBreadcrumb($shortPath, $dirCrumb);
        }
        $offset++;
      }

    }

    if ($fileName !== '') {
      $shortPath = '';
      $breadcrumbs .= $this->getActiveBreadcrumb($shortPath, $fileName);
    }

    return $breadcrumbs;

    //
    // $path = $node->getPathAsArray();
    // $pathToRoot = [];
    // $max = count($path);
    //
    // if ($max > 1) {
    //   var_dump($path);
    //   var_dump($breadcrumbs);
    //   exit();
    // }
    //
    // if ($node instanceof FileNode) {
    //   $max--;
    // }
    //
    // for ($i = 0; $i < $max; $i++) {
    //   $pathToRoot[] = str_repeat('../', $i);
    // }
    //
    // foreach ($path as $step) {
    //   if ($step !== $node) {
    //     $breadcrumbs .=
    //       $this->getInactiveBreadcrumb($step, array_pop($pathToRoot));
    //   } else {
    //     $breadcrumbs .= $this->getActiveBreadcrumb($step);
    //   }
    // }
    //
    // return $breadcrumbs;
  }

  protected function getActiveBreadcrumb(
    string $shortPath,
    string $crumb,
  ): string {
    return sprintf('<li class="active">%s</li>', $crumb);
  }

  protected function getInactiveBreadcrumb(
    string $shortPath,
    string $crumb,
  ): string {
    return
      sprintf('<li><a href="%sindex.html">%s</a></li>', $shortPath, $crumb);
  }

  protected function getPathToRoot(AbstractNode $node): string {
    return '/';
  }

  protected function getCoverageBar(float $percent): string {
    $level = $this->getColorLevel($percent);
    return CoverageBar::render($level, sprintf('%.2F', $percent));
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
