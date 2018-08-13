<?hh // strict

namespace Zynga\CodeBase\V1;

use Zynga\CodeBase\V1\File;
use \Exception;

class FileFactory {
  private static Map<string, File> $files = Map {};

  public static function clear(): void {
    self::$files->clear();
  }

  public static function get(string $filename): File {

    $file = self::$files->get($filename);

    if ($file instanceof File) {
      return $file;
    }

    if (!is_file($filename)) {
      throw new Exception('NOT_A_FILE='.$filename);
    }

    $file = new File($filename);
    $file->source()->load();
    $file->rawTokens()->load();
    $file->stream();
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
