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
use SebastianBergmann\TokenStream\Token\Stream\CachingFactory;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Class;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Interface;
use Zynga\CodeBase\V1\FileFactory;

use \Exception;

class InterfaceTest extends TestCase {

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

  protected function getTestClassAndInterfaces(
  ): (PHP_Token_Class, Vector<PHP_Token_Interface>) {

    $filename = $this->getFilesDirectory().'source4.php';

    $codeFile = FileFactory::get($filename);
    $tokenStream = $codeFile->stream();

    $tokens = $tokenStream->tokens();

    $class = null;
    $interfaces = Vector {};

    foreach ($tokens as $token) {
      if ($token instanceof PHP_Token_Class) {
        $class = $token;
      } else if ($token instanceof PHP_Token_Interface) {
        $interfaces->add($token);
      }
    }

    if (!$class instanceof PHP_Token_Class) {
      throw new Exception('Failed to find target token class');
    }

    return tuple($class, $interfaces);

  }

  public function testGetName(): void {
    list($class, $interfaces) = $this->getTestClassAndInterfaces();
    $this->assertEquals('iTemplate', $interfaces[0]->getName());
  }

  public function testGetParentNotExists(): void {
    list($class, $interfaces) = $this->getTestClassAndInterfaces();
    $this->assertFalse(boolval($interfaces[0]->getParent()));
  }

  public function testHasParentNotExists(): void {
    list($class, $interfaces) = $this->getTestClassAndInterfaces();
    $this->assertFalse($interfaces[0]->hasParent());
  }

  public function testGetParentExists(): void {
    list($class, $interfaces) = $this->getTestClassAndInterfaces();
    $this->assertEquals('a', $interfaces[2]->getParent());
  }

  public function testHasParentExists(): void {
    list($class, $interfaces) = $this->getTestClassAndInterfaces();
    $this->assertTrue($interfaces[2]->hasParent());
  }

  public function testGetInterfacesExists(): void {
    list($class, $interfaces) = $this->getTestClassAndInterfaces();
    $interfaces = $class->getInterfaces();
    if ($interfaces instanceof Vector) {
      $this->assertEquals(['b'], $interfaces->toArray());
    }
  }

  public function testHasInterfacesExists(): void {
    list($class, $interfaces) = $this->getTestClassAndInterfaces();
    $this->assertTrue($class->hasInterfaces());
  }

  public function testGetPackageNamespace(): void {
    $filename = $this->getFilesDirectory().'classInNamespace.php';

    $codeFile = FileFactory::get($filename);
    $tokenStream = $codeFile->stream();

    $tokens = $tokenStream->tokens();
    foreach ($tokens as $token) {
      if ($token instanceof PHP_Token_Interface) {
        $package = $token->getPackage();
        $this->assertSame('Foo\\Bar', $package['namespace']);
      }
    }
  }

  public function testFilesWithClassesWithinMultipleNamespaces(): void {
    $files = Vector {};
    $files->add(
      $this->getFilesDirectory().
      'multipleNamespacesWithOneClassUsingBraces.php',
    );
    $files->add(
      $this->getFilesDirectory().
      'multipleNamespacesWithOneClassUsingNonBraceSyntax.php',
    );

    foreach ($files as $file) {
      $this->doGetPackageNamespaceForFileWithMultipleNamespaces($file);
    }

  }

  public function doGetPackageNamespaceForFileWithMultipleNamespaces(
    string $filepath,
  ): void {

    $codeFile = FileFactory::get($filepath);
    $tokenStream = $codeFile->stream();

    $firstClassFound = false;

    $tokens = $tokenStream->tokens();

    foreach ($tokens as $token) {
      if ($firstClassFound === false &&
          $token instanceof PHP_Token_Interface) {
        $package = $token->getPackage();
        $this->assertSame('TestClassInBar', $token->getName());
        $this->assertSame('Foo\\Bar', $package['namespace']);
        $firstClassFound = true;
        continue;
      }
      // Second class
      if ($token instanceof PHP_Token_Interface) {
        $package = $token->getPackage();
        $this->assertSame('TestClassInBaz', $token->getName());
        $this->assertSame('Foo\\Baz', $package['namespace']);

        return;
      }
    }
    $this->fail('Searching for 2 classes failed');
  }

  public function testGetPackageNamespaceIsEmptyForInterfacesThatAreNotWithinNamespaces(
  ): void {
    list($class, $interfaces) = $this->getTestClassAndInterfaces();
    foreach ($interfaces as $token) {
      $package = $token->getPackage();
      $this->assertSame('', $package['namespace']);
    }
  }

  public function testGetPackageNamespaceWhenExtentingFromNamespaceClass(
  ): void {
    $filename = $this->getFilesDirectory().'classExtendsNamespacedClass.php';

    $codeFile = FileFactory::get($filename);
    $tokenStream = $codeFile->stream();

    $firstClassFound = false;
    $tokens = $tokenStream->tokens();
    foreach ($tokens as $token) {
      if ($firstClassFound === false &&
          $token instanceof PHP_Token_Interface) {
        $package = $token->getPackage();
        $this->assertSame('Baz', $token->getName());
        $this->assertSame('Foo\\Bar', $package['namespace']);
        $firstClassFound = true;
        continue;
      }
      if ($token instanceof PHP_Token_Interface) {
        $package = $token->getPackage();
        $this->assertSame('Extender', $token->getName());
        $this->assertSame('Other\\Space', $package['namespace']);

        return;
      }
    }
    $this->fail('Searching for 2 classes failed');
  }
}
