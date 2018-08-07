<?hh // strict

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
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Function;

class ClosureTest extends TestCase {

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

  protected function getFunctionsForTest(): Vector<PHP_Token_Function> {

    $filename = $this->getFilesDirectory().'closure.php';

    $ts = CachingFactory::get($filename);

    $tokens = $ts->tokens();

    $functions = Vector {};

    foreach ($tokens as $token) {
      if ($token instanceof PHP_Token_Function) {
        $functions->add($token);
      }
    }

    return $functions;

  }

  public function testGetArguments(): void {

    $functions = $this->getFunctionsForTest();

    $this->assertEquals(
      ['$foo' => null, '$bar' => null],
      $functions[0]->getArguments()->toArray(),
    );
    $this->assertEquals(
      ['$foo' => 'Foo', '$bar' => null],
      $functions[1]->getArguments()->toArray(),
    );
    $this->assertEquals(
      ['$foo' => null, '$bar' => null, '$baz' => null],
      $functions[2]->getArguments()->toArray(),
    );
    $this->assertEquals(
      ['$foo' => 'Foo', '$bar' => null, '$baz' => null],
      $functions[3]->getArguments()->toArray(),
    );
    $this->assertEquals([], $functions[4]->getArguments()->toArray());
    $this->assertEquals([], $functions[5]->getArguments()->toArray());
  }

  public function testGetName(): void {

    $functions = $this->getFunctionsForTest();

    $this->assertEquals('anonymous function', $functions[0]->getName());
    $this->assertEquals('anonymous function', $functions[1]->getName());
    $this->assertEquals('anonymous function', $functions[2]->getName());
    $this->assertEquals('anonymous function', $functions[3]->getName());
    $this->assertEquals('anonymous function', $functions[4]->getName());
    $this->assertEquals('anonymous function', $functions[5]->getName());

  }

  public function testGetLine(): void {

    $functions = $this->getFunctionsForTest();
    $this->assertEquals(2, $functions[0]->getLine());
    $this->assertEquals(3, $functions[1]->getLine());
    $this->assertEquals(4, $functions[2]->getLine());
    $this->assertEquals(5, $functions[3]->getLine());

  }

  public function testGetEndLine(): void {
    $functions = $this->getFunctionsForTest();

    $this->assertEquals(2, $functions[0]->getLine());
    $this->assertEquals(3, $functions[1]->getLine());
    $this->assertEquals(4, $functions[2]->getLine());
    $this->assertEquals(5, $functions[3]->getLine());
  }
}
