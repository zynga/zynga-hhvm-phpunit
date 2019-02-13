<?hh // strict

namespace Zynga\PHPUnit\V2\Tests\Framework;

use Zynga\Framework\Environment\CodePath\V1\CodePath;

class BaseTest {

  public static function getFilesDirectory(): string {
    return
      CodePath::getRoot().
      DIRECTORY_SEPARATOR.
      'vendor'.
      DIRECTORY_SEPARATOR.
      'zynga'.
      DIRECTORY_SEPARATOR.
      'phpunit'.
      DIRECTORY_SEPARATOR.
      'tests'.
      DIRECTORY_SEPARATOR.
      'phpunit'.
      DIRECTORY_SEPARATOR.
      '_files'.
      DIRECTORY_SEPARATOR;
  }

}
