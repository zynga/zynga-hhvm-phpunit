<?hh // strict

/*
 * This file is part of the php-code-coverage package.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace SebastianBergmann\CodeCoverage\Node;

use SebastianBergmann\CodeCoverage\CodeCoverage;
use SebastianBergmann\CodeCoverage\ProcessedFile\FileContainer;

class Builder {
  private static ?Directory $_root = null;

  /**
   * @param CodeCoverage $coverage
   *
   * @return Directory
   */
  public function build(CodeCoverage $coverage): Directory {

    if (self::$_root instanceof Directory) {
      return self::$_root;
    }

    $fileNames = FileContainer::getFileNames();

    $dirMap = Map {};

    // establish a entry for the root of the FS.
    $root = new Directory(DIRECTORY_SEPARATOR);

    $dirMap->set(DIRECTORY_SEPARATOR, $root);

    // // Group all the files under their parental directories.
    foreach ($fileNames as $fileName) {

      $directory = dirname($fileName);

      $directoryEntry = $dirMap->get($directory);

      $isNew = false;
      if (!$directoryEntry instanceof Directory) {
        $directoryEntry = new Directory($directory);
        $isNew = true;
      }

      // walk up the directory, to make sure all the nodes are in place for
      // directories above you.
      $t_directory = $directory;
      while ($t_directory != DIRECTORY_SEPARATOR) {
        // move once up the path name
        $t_directory = dirname($t_directory);
        $t_directoryEntry = $dirMap->get($t_directory);
        if (!$t_directoryEntry instanceof Directory) {
          $t_directoryEntry = new Directory($t_directory);
          $dirMap->set($t_directory, $t_directoryEntry);
        }

      }

      // echo "directory=$directory\n";
      // echo "  file=$fileName\n";

      $directoryEntry->addFile($fileName);

      if ($isNew === true) {
        $dirMap->set($directory, $directoryEntry);
      }

    }

    foreach ($dirMap as $directory => $directoryEntry) {

      // Look for direct descendents.
      foreach ($dirMap as $childDirectory => $childDirectoryEntry) {

        if ($this->isChildOfDirectory($directory, $childDirectory) === true) {

          $directoryEntry->addAlreadyCreatedDirectory($childDirectoryEntry);

        }

      }

    }

    self::$_root = $root;

    return self::$_root;

  }

  private function isChildOfDirectory(
    string $directory,
    string $childDirectory,
  ): bool {

    if ($directory == $childDirectory) {
      return false;
    }

    $offset = strpos($childDirectory, $directory);

    // has to lead in with this with this directory
    if ($offset !== false && $offset == 0) {

      // only allow 1 leaf deeper.
      $extractedParent = dirname($childDirectory);

      if ($extractedParent == $directory) {
        return true;
      }

    }

    return false;

  }

}
