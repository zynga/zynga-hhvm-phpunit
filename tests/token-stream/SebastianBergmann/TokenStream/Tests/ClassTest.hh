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
use SebastianBergmann\TokenStream\Token\Stream\CachingFactory;
use SebastianBergmann\TokenStream\Token\Stream;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Class;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Function;

use Zynga\CodeBase\V1\FileFactory;

class ClassTest extends TestCase {

  public function getTestClassAndFunction(
  ): (?PHP_Token_Class, ?PHP_Token_Function) {

    parent::doSetUpBeforeClass();

    $filename = $this->getFilesDirectory().'source2.php';
    $ts = CachingFactory::get($filename);

    $tokens = $ts->tokens();

    $classToken = null;
    $functionToken = null;

    foreach ($tokens as $token) {

      if ($token instanceof PHP_Token_Class) {
        $classToken = $token;
      }

      if ($token instanceof PHP_Token_Function) {
        $functionToken = $token;
        break;
      }

    }

    return tuple($classToken, $functionToken);

  }

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

  public function testGetClassKeywords(): void {

    list($classUnderTest, $functionUnderTest) =
      $this->getTestClassAndFunction();

    if ($classUnderTest instanceof PHP_Token_Class) {
      $this->assertEquals('abstract', $classUnderTest->getKeywords());
    } else {
      $this->fail('test class not available');
    }

  }

  public function testGetFunctionKeywords(): void {

    list($classUnderTest, $functionUnderTest) =
      $this->getTestClassAndFunction();

    if ($functionUnderTest instanceof PHP_Token_Function) {

      $this->assertEquals(
        'abstract,static',
        $functionUnderTest->getKeywords(),
      );

    } else {
      $this->fail('test function not available');
    }

  }

  public function testGetFunctionVisibility(): void {

    list($classUnderTest, $functionUnderTest) =
      $this->getTestClassAndFunction();

    if ($functionUnderTest instanceof PHP_Token_Function) {
      $this->assertEquals('public', $functionUnderTest->getVisibility());
    } else {
      $this->fail('test function not available');
    }

  }

  public function testIssue19(): void {
    $filename = $this->getFilesDirectory().'issue19.hh';
    $ts = CachingFactory::get($filename);
    foreach ($ts->tokens() as $token) {
      if ($token instanceof PHP_Token_Class) {
        $this->assertFalse($token->hasInterfaces());
      }
    }
  }

  public function testIssue30(): void {

    $filename = $this->getFilesDirectory().'issue30.php';

    $codeFile = FileFactory::get($filename);
    $this->assertCount(1, $codeFile->classes()->getAll());

  }

  public function testAnonymousClassesAreHandledCorrectly(): void {

    $filename =
      $this->getFilesDirectory().
      'class_with_method_that_declares_anonymous_class.php';

    $codeFile = FileFactory::get($filename);

    $classes = $codeFile->classes()->getAll();

    $this->assertTrue(
      $classes->containsKey(
        'class_with_method_that_declares_anonymous_class',
      ),
    );

  }

  /**
   * @ticket   https://github.com/sebastianbergmann/php-token-stream/issues/52
   */
  // JEO: This php7 support isn't completed in the version of hhvm we are on.
  /*
   public function testAnonymousClassesAreHandledCorrectly2(): void {
   $filename =
   $this->getFilesDirectory().
   'class_with_method_that_declares_anonymous_class2.php'
   ;
   $ts = CachingFactory::get($filename);
   $classes = $ts->getClasses();
   $this->assertEquals(['Test'], array_keys($classes->toArray()));
   $this->assertEquals(
   ['methodOne', 'methodTwo'],
   array_keys($classes['Test']->methods->toArray()),
   );
   $this->assertEmpty($ts->getFunctions());
   }
   */

  /**
   * @requires PHP 5.6
   */
  public function testImportedFunctionsAreHandledCorrectly(): void {
    $filename = $this->getFilesDirectory().'classUsesNamespacedFunction.php';
    $codeFile = FileFactory::get($filename);
    $this->assertEmpty($codeFile->functions()->getAll());
    $this->assertCount(1, $codeFile->classes()->getAll());
  }
}
