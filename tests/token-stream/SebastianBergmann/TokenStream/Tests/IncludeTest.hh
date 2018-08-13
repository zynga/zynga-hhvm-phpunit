<?hh // strict

namespace SebastianBergmann\TokenStream\Tests;

/*
 * This file is part of php-token-stream.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use Zynga\Framework\Testing\TestCase\V2\Base as TestCase;
use Zynga\Framework\Environment\CodePath\V1\CodePath;
use SebastianBergmann\TokenStream\Token\Stream;
use Zynga\CodeBase\V1\FileFactory;
use Zynga\CodeBase\V1\File;

class PHP_Token_IncludeTest extends TestCase {

  protected function getFilesDirectory(): string {
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
      'token-stream'.
      DIRECTORY_SEPARATOR.
      '_fixture'.
      DIRECTORY_SEPARATOR;
  }

  protected function getTestFile(): File {
    $filename = $this->getFilesDirectory().'source3.php';
    return FileFactory::get($filename);
  }

  public function testGetIncludes(): void {

    $codeFile = $this->getTestFile();

    $includes = $codeFile->inclusions()->getAll();

    if ($includes instanceof Vector) {
      $this->assertSame(
        ['test4.php', 'test3.php', 'test2.php', 'test1.php'],
        $includes->toArray(),
      );
    }

  }

  public function testGetIncludesCategorized(): void {

    $codeFile = $this->getTestFile();

    $includes = $codeFile->inclusions()->getAllAsMap();

    if ($includes instanceof Map) {

      $a_includes = array();
      foreach ($includes as $includeKey => $includeValue) {

        if ($includeValue instanceof Vector) {
          $a_includes[$includeKey] = $includeValue->toArray();
        }

      }

      $this->assertSame(
        [
          'require_once' => ['test4.php'],
          'require' => ['test3.php'],
          'include_once' => ['test2.php'],
          'include' => ['test1.php'],
        ],
        $a_includes,
      );
    }

  }

  public function testGetIncludesCategory(): void {

    $codeFile = $this->getTestFile();

    $includes = $codeFile->inclusions()->getSpecificType('require_once');

    if ($includes instanceof Vector) {
      $this->assertSame(['test4.php'], $includes->toArray());
    }

  }
}
