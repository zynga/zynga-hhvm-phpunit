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

use SebastianBergmann\CodeCoverage\CodeCoverage;
use SebastianBergmann\CodeCoverage\Node\Directory as NodeDirectory;
use SebastianBergmann\CodeCoverage\Report\Html\Dashboard;
use SebastianBergmann\CodeCoverage\Report\Html\Renderer\Directory;
use SebastianBergmann\CodeCoverage\Report\Html\Renderer\File;
use \RuntimeException;

/**
 * Generates an HTML report from a code coverage object.
 */
class Facade {
  /**
   * @var string
   */
  private string $templatePath;

  /**
   * @var string
   */
  private string $generator;

  /**
   * @var int
   */
  private int $lowUpperBound;

  /**
   * @var int
   */
  private int $highLowerBound;

  /**
   * Constructor.
   *
   * @param int    $lowUpperBound
   * @param int    $highLowerBound
   * @param string $generator
   */
  public function __construct(
    int $lowUpperBound = 50,
    int $highLowerBound = 90,
    string $generator = '',
  ) {
    $this->generator = $generator;
    $this->highLowerBound = $highLowerBound;
    $this->lowUpperBound = $lowUpperBound;
    $this->templatePath = __DIR__.'/Renderer/Template/';
  }

  /**
   * @param CodeCoverage $coverage
   * @param string       $target
   */
  public function process(CodeCoverage $coverage, string $target): void {

    // --
    // Find the first child node that has more than 1 directory / files.
    // --
    $root = $coverage->getReport();
    $root = $this->huntForNonSingularDirectory($root);

    $currentTime = time();
    $date = date('D M j G:i:s T Y', $currentTime);

    if ($root instanceof NodeDirectory) {
      $this->doProcess($root, $target, $date);
    }

  }

  private function huntForNonSingularDirectory(
    NodeDirectory $root,
  ): ?NodeDirectory {

    if ($root->getDirectories()->count() > 1 ||
        $root->getFiles()->count() > 0) {
      return $root;
    }

    foreach ($root->getDirectories() as $dir) {
      $newRoot = $this->huntForNonSingularDirectory($dir);
      if ($newRoot instanceof NodeDirectory) {
        return $newRoot;
      }
    }

    return null;

  }

  private function doProcess(
    NodeDirectory $root,
    string $target,
    string $date,
  ): void {

    $target = $this->getDirectory($target);
    $rootPath = $root->getPath();

    $dashboard = new Dashboard(
      $this->templatePath,
      $this->generator,
      $date,
      $this->lowUpperBound,
      $this->highLowerBound,
    );

    $directory = new Directory(
      $this->templatePath,
      $this->generator,
      $date,
      $this->lowUpperBound,
      $this->highLowerBound,
    );

    $file = new File(
      $this->templatePath,
      $this->generator,
      $date,
      $this->lowUpperBound,
      $this->highLowerBound,
    );

    // Recalculate the trees stats after all the muckery we did with it earlier.
    $root->getNumClasses(true);

    // First render ourselves.
    $directory->render($root, $target.'index.html');
    $dashboard->render($root, $target.'dashboard.html');

    $this->_renderDirectory($target, $root, $directory, $dashboard, $file);

    $this->copyFiles($target);

  }

  private function _joinPath(string $currentPath, string $newElems): string {
    if (preg_match('/\/$/', $currentPath)) {
      return $currentPath.$newElems;
    }
    return $currentPath.DIRECTORY_SEPARATOR.$newElems;
  }

  private function _renderDirectory(
    string $target,
    NodeDirectory $root,
    Directory $directory,
    Dashboard $dashboard,
    File $file,
  ): void {

    foreach ($root->getDirectories() as $node) {

      $id = $node->getId();

      $renderDir = $this->_joinPath($target, $id);

      if (!is_dir($renderDir)) {
        mkdir($renderDir, 0755, true);
      }

      $directory->render($node, $this->_joinPath($renderDir, 'index.html'));
      $dashboard->render(
        $node,
        $this->_joinPath($renderDir, 'dashboard.html'),
      );

      $this->_renderDirectory(
        $renderDir,
        $node,
        $directory,
        $dashboard,
        $file,
      );

    }

    foreach ($root->getFiles() as $node) {

      $id = $node->getId();

      $renderDir = $this->_joinPath($target, $id);

      $dir = dirname($renderDir);

      if (!is_dir($dir)) {
        mkdir($dir, 0755, true);
      }

      $file->render($node, $renderDir.'.html');

    }

  }

  private function _needsTemplateCopy(string $target): bool {

    $destVersionFile = $target.'/version.txt';

    if (!is_file($destVersionFile)) {

      return true;
    }

    $sourceVersionFile = $this->templatePath.'/version.txt';

    $destVersionInfo = file_get_contents($destVersionFile);
    $sourceVersionInfo = file_get_contents($sourceVersionFile);

    if ($destVersionInfo == $sourceVersionInfo) {
      return false;
    }

    return true;

  }

  /**
   * @param string $target
   */
  private function copyFiles(string $target): void {

    if ($this->_needsTemplateCopy($target) === false) {
      echo "  copyFiles: skipping shared html components.\n";
      return;
    }

    echo "  copyFiles: copying shared html components\n";

    $root = $this->getDirectory($target);

    copy($this->templatePath.'version.txt', $root.'version.txt');

    $dir = $this->getDirectory($target.'css');
    copy(
      $this->templatePath.'css/bootstrap.min.css',
      $dir.'bootstrap.min.css',
    );
    copy($this->templatePath.'css/nv.d3.min.css', $dir.'nv.d3.min.css');
    copy($this->templatePath.'css/style.css', $dir.'style.css');

    $dir = $this->getDirectory($target.'fonts');
    copy(
      $this->templatePath.'fonts/glyphicons-halflings-regular.eot',
      $dir.'glyphicons-halflings-regular.eot',
    );
    copy(
      $this->templatePath.'fonts/glyphicons-halflings-regular.svg',
      $dir.'glyphicons-halflings-regular.svg',
    );
    copy(
      $this->templatePath.'fonts/glyphicons-halflings-regular.ttf',
      $dir.'glyphicons-halflings-regular.ttf',
    );
    copy(
      $this->templatePath.'fonts/glyphicons-halflings-regular.woff',
      $dir.'glyphicons-halflings-regular.woff',
    );
    copy(
      $this->templatePath.'fonts/glyphicons-halflings-regular.woff2',
      $dir.'glyphicons-halflings-regular.woff2',
    );

    $dir = $this->getDirectory($target.'js');
    copy($this->templatePath.'js/bootstrap.min.js', $dir.'bootstrap.min.js');
    copy($this->templatePath.'js/d3.min.js', $dir.'d3.min.js');
    copy($this->templatePath.'js/holder.min.js', $dir.'holder.min.js');
    copy($this->templatePath.'js/html5shiv.min.js', $dir.'html5shiv.min.js');
    copy($this->templatePath.'js/jquery.min.js', $dir.'jquery.min.js');
    copy($this->templatePath.'js/nv.d3.min.js', $dir.'nv.d3.min.js');
    copy($this->templatePath.'js/respond.min.js', $dir.'respond.min.js');
  }

  /**
   * @param string $directory
   *
   * @return string
   *
   * @throws RuntimeException
   */
  private function getDirectory(string $directory): string {
    if (substr($directory, -1, 1) != DIRECTORY_SEPARATOR) {
      $directory .= DIRECTORY_SEPARATOR;
    }

    if (is_dir($directory)) {
      return $directory;
    }

    if (@mkdir($directory, 0755, true)) {
      return $directory;
    }

    throw new RuntimeException(
      sprintf('Directory "%s" does not exist.', $directory),
    );
  }
}
