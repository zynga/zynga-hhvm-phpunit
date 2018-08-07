<?hh // strict

namespace SebastianBergmann\CodeCoverage\ProcessedFile;

use SebastianBergmann\CodeCoverage\ProcessedFile\ProcessedFile;
use \Exception;

class FileContainer {
  private static Map<string, ProcessedFile> $files = Map {};

  public static function clear(): void {
    self::$files->clear();
  }

  public static function get(string $filename): ProcessedFile {

    $file = self::$files->get($filename);

    if ($file instanceof ProcessedFile) {
      return $file;
    }

    if ( ! is_file($filename) ) {
      throw new Exception('NOT_A_FILE=' . $filename);
    }

    $file = new ProcessedFile($filename);
    $file->init();

    self::$files->set($filename, $file);

    return $file;

  }

  public static function getFileNames(): Vector<string> {
    return self::$files->keys();
  }

  public static function getAllLineToTestData(
  ): array<string, array<int, array<string>>> {
    $data = array();

    foreach (self::$files as $fileName => $fileObj) {
      $data[$fileName] = $fileObj->lineToTestToArrayFormat();
    }

    return $data;
  }

}
