<?hh

namespace Zynga\PHPUnit\V2\Tests\System;

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use Zynga\PHPUnit\V2\Exceptions\AssertionFailedException;
use Zynga\PHPUnit\V2\Exceptions\AttributeNotFoundException;
use Zynga\PHPUnit\V2\Exceptions\InvalidArgumentException;
use Zynga\PHPUnit\V2\Exceptions\TestError\IncompleteException;
use Zynga\PHPUnit\V2\Exceptions\TestError\SkippedException;

use Zynga\Framework\Environment\CodePath\V1\CodePath;
use Zynga\PHPUnit\V2\Tests\Mock\Author;
use Zynga\PHPUnit\V2\Tests\Mock\Book;
use Zynga\PHPUnit\V2\Tests\Mock\ClassWithNonPublicAttributes;
use Zynga\PHPUnit\V2\Tests\Mock\ClassWithToString;
use Zynga\PHPUnit\V2\Tests\Mock\SampleClass;
use Zynga\PHPUnit\V2\Tests\Mock\Struct;
use Zynga\PHPUnit\V2\Tests\Mock\TestIterator;
use Zynga\PHPUnit\V2\TestCase;

use \PHPUnit_Framework_ExpectationFailedException;
use \PHPUnit_Framework_Exception;
use \PHPUnit_Util_XML;

// External classes:
use \ArrayIterator;
use \DateTime;
use \DateTimeZone;
use \DOMDocument;
use \DOMElement;
use \Exception;
use \SplObjectStorage;
use \stdClass;

/**
 * @since      Class available since Release 2.0.0
 */
class AssertTest extends TestCase {

  public function doSetUpBeforeClass(): void {
    // noop
  }

  public function doTearDownAfterClass(): void {
    // noop
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
      'phpunit'.
      DIRECTORY_SEPARATOR.
      '_files'.
      DIRECTORY_SEPARATOR;
  }

  public function testFail(): void {
    try {
      $this->fail();
    } catch (AssertionFailedException $e) {
      $this->assertTrue(true);
      return;
    }
    throw new AssertionFailedException('Fail did not throw fail exception');
  }

  public function testAssertSplObjectStorageContainsObject() {
    $a = new stdClass();
    $b = new stdClass();
    $c = new SplObjectStorage();
    $c->attach($a);

    $this->assertContains($a, $c);

    try {
      $this->assertContains($b, $c);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertArrayContainsObject(): void {
    $a = new stdClass();
    $b = new stdClass();

    $this->assertContains($a, [$a]);

    try {
      $this->assertContains($a, [$b]);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail('Expected exceptions for test were not thrown');
  }

  public function testAssertArrayContainsString(): void {
    $this->assertContains('foo', ['foo']);

    try {
      $this->assertContains('foo', ['bar']);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertArrayContainsNonObject(): void {
    $this->assertContains('foo', [true]);

    try {
      $this->assertContains('foo', [true], '', false, true, true);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertContainsOnlyInstancesOf(): void {
    $test = [new Book(), new Book()];
    $this->assertContainsOnlyInstancesOf(Book::class, $test);
    $this->assertContainsOnlyInstancesOf(stdClass::class, [new stdClass()]);

    $test2 = [new Author('Test')];
    try {
      $this->assertContainsOnlyInstancesOf(Book::class, $test2);
    } catch (AssertionFailedException $e) {
      return;
    }
    $this->fail();
  }

  public function testAssertArrayHasIntegerKey() {
    $this->assertArrayHasKey(0, ['foo']);

    try {
      $this->assertArrayHasKey(1, ['foo']);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertArraySubset() {
    $array = [
      'a' => 'item a',
      'b' => 'item b',
      'c' => ['a2' => 'item a2', 'b2' => 'item b2'],
      'd' => ['a2' => ['a3' => 'item a3', 'b3' => 'item b3']],
    ];

    $this->assertArraySubset(
      ['a' => 'item a', 'c' => ['a2' => 'item a2']],
      $array,
    );
    $this->assertArraySubset(
      ['a' => 'item a', 'd' => ['a2' => ['b3' => 'item b3']]],
      $array,
    );

    try {
      $this->assertArraySubset(['a' => 'bad value'], $array);
    } catch (AssertionFailedException $e) {
    }

    try {
      $this->assertArraySubset(
        ['d' => ['a2' => ['bad index' => 'item b3']]],
        $array,
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertArraySubsetWithDeepNestedArrays() {
    $array = ['path' => ['to' => ['the' => ['cake' => 'is a lie']]]];

    $this->assertArraySubset(['path' => []], $array);
    $this->assertArraySubset(['path' => ['to' => []]], $array);
    $this->assertArraySubset(['path' => ['to' => ['the' => []]]], $array);
    $this->assertArraySubset(
      ['path' => ['to' => ['the' => ['cake' => 'is a lie']]]],
      $array,
    );

    try {
      $this->assertArraySubset(
        ['path' => ['to' => ['the' => ['cake' => 'is not a lie']]]],
        $array,
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertArraySubsetWithNoStrictCheckAndObjects() {
    $obj = new stdClass();
    $reference = &$obj;
    $array = ['a' => $obj];

    $this->assertArraySubset(['a' => $reference], $array);
    $this->assertArraySubset(['a' => new stdClass()], $array);
  }

  public function testAssertArraySubsetWithStrictCheckAndObjects() {
    $obj = new stdClass();
    $reference = &$obj;
    $array = ['a' => $obj];

    $this->assertArraySubset(['a' => $reference], $array, true);

    try {
      $this->assertArraySubset(['a' => new stdClass()], $array, true);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail('Strict recursive array check fail.');
  }

  <<dataProvider("assertArraySubsetInvalidArgumentProvider")>>
  public function testAssertArraySubsetRaisesExceptionForInvalidArguments(
    mixed $partial,
    mixed $subject,
  ): void {
    $message = 'array or ArrayAccess';
    try {
      $this->assertArraySubset($partial, $subject);
    } catch (PHPUnit_framework_Exception $e) {
      $this->assertStringEndsWith($message, $e->getMessage());
      return;
    } catch (InvalidArgumentException $e) {
      $this->assertStringEndsWith($message, $e->getMessage());
      return;
    }
    $this->fail();
  }

  public function testAssertArrayNotHasIntegerKey() {
    $this->assertArrayNotHasKey(1, ['foo']);

    try {
      $this->assertArrayNotHasKey(0, ['foo']);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertArrayHasStringKey() {
    $this->assertArrayHasKey('foo', ['foo' => 'bar']);

    try {
      $this->assertArrayHasKey('bar', ['foo' => 'bar']);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertArrayNotHasStringKey() {

    $this->assertArrayNotHasKey('bar', ['foo' => 'bar']);

    try {
      $this->assertArrayNotHasKey('foo', ['foo' => 'bar']);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertContainsThrowsException() {
    try {
      $this->assertContains(null, null);
    } catch (InvalidArgumentException $e) {
      $this->assertTrue(true);
      return;
    } catch (PHPUnit_Framework_Exception $e) {
      $this->assertTrue(true);
      return;
    }
    $this->fail();
  }

  public function testAssertIteratorContainsObject() {
    $foo = new stdClass();

    $this->assertContains($foo, new TestIterator([$foo]));

    try {
      $this->assertContains($foo, new TestIterator([new stdClass()]));
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertIteratorContainsString() {

    $this->assertContains('foo', new TestIterator(['foo']));

    try {
      $this->assertContains('foo', new TestIterator(['bar']));
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertStringContainsString() {
    $this->assertContains('foo', 'foobar');

    try {
      $this->assertContains('foo', 'bar');
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertNotContainsThrowsException() {
    try {
      $this->assertNotContains(null, null);
    } catch (InvalidArgumentException $e) {
      $this->assertTrue(true);
      return;
    } catch (PHPUnit_Framework_Exception $e) {
      $this->assertTrue(true);
      return;
    }
    $this->fail('Failed to find an acceptable exception.');
  }

  public function testAssertSplObjectStorageNotContainsObject() {
    $a = new stdClass();
    $b = new stdClass();
    $c = new SplObjectStorage();
    $c->attach($a);

    $this->assertNotContains($b, $c);

    try {
      $this->assertNotContains($a, $c);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertArrayNotContainsObject() {
    $a = new stdClass();
    $b = new stdClass();

    $this->assertNotContains($a, [$b]);

    try {
      $this->assertNotContains($a, [$a]);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertArrayNotContainsString() {
    $this->assertNotContains('foo', ['bar']);

    try {
      $this->assertNotContains('foo', ['foo']);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertArrayNotContainsNonObject() {
    $this->assertNotContains('foo', [true], '', false, true, true);

    try {
      $this->assertNotContains('foo', [true]);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertStringNotContainsString() {
    $this->assertNotContains('foo', 'bar');

    try {
      $this->assertNotContains('foo', 'foo');
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertArrayContainsOnlyIntegers() {
    $this->assertContainsOnly('integer', [1, 2, 3]);

    try {
      $this->assertContainsOnly('integer', ['1', 2, 3]);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail('Failed to catch appropriate exceptions.');
  }

  public function testAssertArrayNotContainsOnlyIntegers() {
    $this->assertNotContainsOnly('integer', ['1', 2, 3]);

    try {
      $this->assertNotContainsOnly('integer', [1, 2, 3]);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertArrayContainsOnlyStdClass() {
    $this->assertContainsOnly('StdClass', [new stdClass()]);

    try {
      $this->assertContainsOnly('StdClass', ['StdClass']);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail('Failed to catch appropriate exceptions.');

  }

  public function testAssertArrayNotContainsOnlyStdClass() {
    $this->assertNotContainsOnly('StdClass', ['StdClass']);

    try {
      $this->assertNotContainsOnly('StdClass', [new stdClass()]);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  <<dataProvider("equalProvider")>>
  public function testAssertEqualsSucceeds(
    mixed $a,
    mixed $b,
    float $delta = 0.0,
    bool $canonicalize = false,
    bool $ignoreCase = false,
  ): void {
    if (is_float($delta) != true) {
      $delta = floatval($delta);
    }
    $this->assertEquals($a, $b, '', $delta, 10, $canonicalize, $ignoreCase);
  }

  <<dataProvider("notEqualProvider")>>
  public function testAssertEqualsFails(
    mixed $a,
    mixed $b,
    float $delta = 0.0,
    bool $canonicalize = false,
    bool $ignoreCase = false,
  ): void {
    try {
      if (is_float($delta) != true) {
        $delta = floatval($delta);
      }
      $this->assertEquals($a, $b, '', $delta, 10, $canonicalize, $ignoreCase);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  <<dataProvider("notEqualProvider")>>
  public function testAssertNotEqualsSucceeds(
    mixed $a,
    mixed $b,
    float $delta = 0.0,
    bool $canonicalize = false,
    bool $ignoreCase = false,
  ): void {
    if (is_float($delta) != true) {
      $delta = floatval($delta);
    }
    $this->assertNotEquals(
      $a,
      $b,
      '',
      $delta,
      10,
      $canonicalize,
      $ignoreCase,
    );
  }

  <<dataProvider("equalProvider")>>
  public function testAssertNotEqualsFails(
    mixed $a,
    mixed $b,
    float $delta = 0.0,
    bool $canonicalize = false,
    bool $ignoreCase = false,
  ): void {
    try {
      if (is_float($delta) != true) {
        $delta = floatval($delta);
      }
      $this->assertNotEquals(
        $a,
        $b,
        '',
        $delta,
        10,
        $canonicalize,
        $ignoreCase,
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  <<dataProvider("sameProvider")>>
  public function testAssertSameSucceeds(mixed $a, mixed $b): void {
    $this->assertSame($a, $b);
  }

  <<dataProvider("notSameProvider")>>
  public function testAssertSameFails(mixed $a, mixed $b): void {
    try {
      $this->assertSame($a, $b);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  <<dataProvider("notSameProvider")>>
  public function testAssertNotSameSucceeds(mixed $a, mixed $b): void {
    $this->assertNotSame($a, $b);
  }

  <<dataProvider("sameProvider")>>
  public function testAssertNotSameFails(mixed $a, mixed $b): void {
    try {
      $this->assertNotSame($a, $b);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertXmlFileEqualsXmlFile() {
    $this->assertXmlFileEqualsXmlFile(
      $this->getFilesDirectory().'foo.xml',
      $this->getFilesDirectory().'foo.xml',
    );

    try {
      $this->assertXmlFileEqualsXmlFile(
        $this->getFilesDirectory().'foo.xml',
        $this->getFilesDirectory().'bar.xml',
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertXmlFileNotEqualsXmlFile() {
    $this->assertXmlFileNotEqualsXmlFile(
      $this->getFilesDirectory().'foo.xml',
      $this->getFilesDirectory().'bar.xml',
    );

    try {
      $this->assertXmlFileNotEqualsXmlFile(
        $this->getFilesDirectory().'foo.xml',
        $this->getFilesDirectory().'foo.xml',
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertXmlStringEqualsXmlFile() {
    $this->assertXmlStringEqualsXmlFile(
      $this->getFilesDirectory().'foo.xml',
      file_get_contents($this->getFilesDirectory().'foo.xml'),
    );

    try {
      $this->assertXmlStringEqualsXmlFile(
        $this->getFilesDirectory().'foo.xml',
        file_get_contents($this->getFilesDirectory().'bar.xml'),
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testXmlStringNotEqualsXmlFile() {
    $this->assertXmlStringNotEqualsXmlFile(
      $this->getFilesDirectory().'foo.xml',
      file_get_contents($this->getFilesDirectory().'bar.xml'),
    );

    try {
      $this->assertXmlStringNotEqualsXmlFile(
        $this->getFilesDirectory().'foo.xml',
        file_get_contents($this->getFilesDirectory().'foo.xml'),
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertXmlStringEqualsXmlString() {
    $this->assertXmlStringEqualsXmlString('<root/>', '<root/>');

    try {
      $this->assertXmlStringEqualsXmlString('<foo/>', '<bar/>');
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertXmlStringEqualsXmlString2() {
    try {
      $this->assertXmlStringEqualsXmlString('<a></b>', '<c></d>');
    } catch (AssertionFailedException $e) {
      $this->assertTrue(true);
      return;
    } catch (PHPUnit_Framework_Exception $e) {
      $this->assertTrue(true);
      return;
    }
    $this->fail();
  }

  /**
   * @ticket 1860
   */
  public function testAssertXmlStringEqualsXmlString3() {
    $expected = <<<XML
<?xml version="1.0"?>
<root>
    <node />
</root>
XML;

    $actual = <<<XML
<?xml version="1.0"?>
<root>
<node />
</root>
XML;

    $this->assertXmlStringEqualsXmlString($expected, $actual);
  }

  public function testAssertXmlStringNotEqualsXmlString() {
    $this->assertXmlStringNotEqualsXmlString('<foo/>', '<bar/>');

    try {
      $this->assertXmlStringNotEqualsXmlString('<root/>', '<root/>');
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testXMLStructureIsSame() {
    $expected = new DOMDocument();
    $expected->load($this->getFilesDirectory().'structureExpected.xml');

    $actual = new DOMDocument();
    $actual->load($this->getFilesDirectory().'structureExpected.xml');

    $expectedFirstChild = $expected->firstChild;
    $actualFirstChild = $actual->firstChild;

    if ($expectedFirstChild instanceof DOMElement &&
        $actualFirstChild instanceof DOMElement) {

      $this->assertEqualXMLStructure(
        $expectedFirstChild,
        $actualFirstChild,
        true,
      );

      return;
    }

    $this->fail('assertEqualXMLStructure not called');

  }

  public function testXMLStructureWrongNumberOfAttributes() {

    try {
      $expected = new DOMDocument();
      $expected->load($this->getFilesDirectory().'structureExpected.xml');

      $actual = new DOMDocument();
      $actual->load(
        $this->getFilesDirectory().'structureWrongNumberOfAttributes.xml',
      );

      $expectedFirstChild = $expected->firstChild;
      $actualFirstChild = $actual->firstChild;

      //var_dump(get_class($expectedFirstChild));
      //var_dump(get_class($actualFirstChild));

      if ($expectedFirstChild instanceof DOMElement &&
          $actualFirstChild instanceof DOMElement) {

        $this->assertEqualXMLStructure(
          $expectedFirstChild,
          $actualFirstChild,
          true,
        );

        return;
      }

      $this->fail();

    } catch (AssertionFailedException $e) {
      $this->assertTrue(true);
      return;
    } catch (PHPUnit_Framework_ExpectationFailedException $e) {
      $this->assertTrue(true);
      return;
    }

    $this->fail('Failed emitting assertion failure exception.');

  }

  public function testXMLStructureWrongNumberOfNodes() {
    try {
      $expected = new DOMDocument();
      $expected->load($this->getFilesDirectory().'structureExpected.xml');

      $actual = new DOMDocument();
      $actual->load(
        $this->getFilesDirectory().'structureWrongNumberOfNodes.xml',
      );

      $expectedFirstChild = $expected->firstChild;
      $actualFirstChild = $actual->firstChild;

      if ($expectedFirstChild instanceof DOMElement &&
          $actualFirstChild instanceof DOMElement) {

        $this->assertEqualXMLStructure(
          $expectedFirstChild,
          $actualFirstChild,
          true,
        );

      }

    } catch (AssertionFailedException $e) {
      return;
    } catch (PHPUnit_Framework_ExpectationFailedException $e) {
      return;
    }

    $this->fail('assertEqualXMLStructure did not run');

  }

  public function testXMLStructureIsSameButDataIsNot() {
    $expected = new DOMDocument();
    $expected->load($this->getFilesDirectory().'structureExpected.xml');

    $actual = new DOMDocument();
    $actual->load(
      $this->getFilesDirectory().'structureIsSameButDataIsNot.xml',
    );

    $expectedFirstChild = $expected->firstChild;
    $actualFirstChild = $actual->firstChild;

    // var_dump(get_class($expectedFirstChild));
    // var_dump(get_class($actualFirstChild));

    if ($expectedFirstChild instanceof DOMElement &&
        $actualFirstChild instanceof DOMElement) {

      $this->assertEqualXMLStructure(
        $expectedFirstChild,
        $actualFirstChild,
        true,
      );

      return;

    }

    $this->fail('assertEqualXMLStructure was not called');

  }

  public function testXMLStructureAttributesAreSameButValuesAreNot() {
    $expected = new DOMDocument();
    $expected->load($this->getFilesDirectory().'structureExpected.xml');

    $actual = new DOMDocument();
    $actual->load(
      $this->getFilesDirectory().
      'structureAttributesAreSameButValuesAreNot.xml',
    );

    $expectedFirstChild = $expected->firstChild;
    $actualFirstChild = $actual->firstChild;

    //var_dump(get_class($expectedFirstChild));
    //var_dump(get_class($actualFirstChild));

    if ($expectedFirstChild instanceof DOMElement &&
        $actualFirstChild instanceof DOMElement) {
      $this->assertEqualXMLStructure(
        $expectedFirstChild,
        $actualFirstChild,
        true,
      );
      return;
    }

    $this->fail('assertEqualXMLStructure was not ran');

  }

  public function testXMLStructureIgnoreTextNodes() {
    $expected = new DOMDocument();
    $expected->load($this->getFilesDirectory().'structureExpected.xml');

    $actual = new DOMDocument();
    $actual->load($this->getFilesDirectory().'structureIgnoreTextNodes.xml');

    $expectedFirstChild = $expected->firstChild;
    $actualFirstChild = $actual->firstChild;

    //var_dump(get_class($expectedFirstChild));
    //var_dump(get_class($actualFirstChild));

    if ($expectedFirstChild instanceof DOMElement &&
        $actualFirstChild instanceof DOMElement) {
      $this->assertEqualXMLStructure(
        $expectedFirstChild,
        $actualFirstChild,
        true,
      );
      return;
    }

    $this->fail('assertEqualXMLStructure was not called');

  }

  public function testAssertStringEqualsNumeric() {
    $this->assertEquals('0', 0);

    try {
      $this->assertEquals('0', 1);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertStringEqualsNumeric2() {
    $this->assertNotEquals('A', 0);
  }

  public function testAssertFileExists() {
    $this->assertFileExists(__FILE__);

    try {
      $this->assertFileExists(__DIR__.DIRECTORY_SEPARATOR.'NotExisting');
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertFileNotExists() {
    $this->assertFileNotExists(__DIR__.DIRECTORY_SEPARATOR.'NotExisting');

    try {
      $this->assertFileNotExists(__FILE__);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertObjectHasAttribute() {
    $o = new Author('Terry Pratchett');

    $this->assertObjectHasAttribute('name', $o);

    try {
      $this->assertObjectHasAttribute('foo', $o);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertObjectNotHasAttribute() {
    $o = new Author('Terry Pratchett');

    $this->assertObjectNotHasAttribute('foo', $o);

    try {
      $this->assertObjectNotHasAttribute('name', $o);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertFinite() {
    $this->assertFinite(1);

    try {
      $this->assertFinite(INF);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertInfinite() {
    $this->assertInfinite(INF);

    try {
      $this->assertInfinite(1);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertNan() {
    $this->assertNan(NAN);

    try {
      $this->assertNan(1);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertNull() {
    $this->assertNull(null);

    try {
      $this->assertNull(new stdClass());
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertNotNull() {
    $this->assertNotNull(new stdClass());

    try {
      $this->assertNotNull(null);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertTrue() {
    $this->assertTrue(true);

    try {
      $this->assertTrue(false);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertNotTrue() {

    $this->assertNotTrue(false);

    try {
      $this->assertNotTrue(true);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertFalse() {

    $this->assertFalse(false);

    try {
      $this->assertFalse(true);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertNotFalse() {
    $this->assertNotFalse(true);

    try {
      $this->assertNotFalse(false);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertRegExp() {
    $this->assertRegExp('/foo/', 'foobar');

    try {
      $this->assertRegExp('/foo/', 'bar');
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertNotRegExp() {
    $this->assertNotRegExp('/foo/', 'bar');

    try {
      $this->assertNotRegExp('/foo/', 'foobar');
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertSame() {
    $o = new stdClass();

    $this->assertSame($o, $o);

    try {
      $this->assertSame(new stdClass(), new stdClass());
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertSame2() {
    $this->assertSame(true, true);
    $this->assertSame(false, false);

    try {
      $this->assertSame(true, false);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertNotSame() {
    $this->assertNotSame(new stdClass(), null);

    $this->assertNotSame(null, new stdClass());

    $this->assertNotSame(new stdClass(), new stdClass());

    $o = new stdClass();

    try {
      $this->assertNotSame($o, $o);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertNotSame2() {
    $this->assertNotSame(true, false);
    $this->assertNotSame(false, true);

    try {
      $this->assertNotSame(true, true);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertNotSameFailsNull() {
    try {
      $this->assertNotSame(null, null);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testGreaterThan() {
    $this->assertGreaterThan(1, 2);

    try {
      $this->assertGreaterThan(2, 1);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAttributeGreaterThan() {
    $this->assertAttributeGreaterThan(
      1,
      'bar',
      new ClassWithNonPublicAttributes(),
    );

    try {
      $this->assertAttributeGreaterThan(
        1,
        'foo',
        new ClassWithNonPublicAttributes(),
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testGreaterThanOrEqual() {
    $this->assertGreaterThanOrEqual(1, 2);

    try {
      $this->assertGreaterThanOrEqual(2, 1);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAttributeGreaterThanOrEqual() {
    $this->assertAttributeGreaterThanOrEqual(
      1,
      'bar',
      new ClassWithNonPublicAttributes(),
    );

    try {
      $this->assertAttributeGreaterThanOrEqual(
        2,
        'foo',
        new ClassWithNonPublicAttributes(),
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testLessThan() {
    $this->assertLessThan(2, 1);

    try {
      $this->assertLessThan(1, 2);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAttributeLessThan() {
    $this->assertAttributeLessThan(
      2,
      'foo',
      new ClassWithNonPublicAttributes(),
    );

    try {
      $this->assertAttributeLessThan(
        1,
        'bar',
        new ClassWithNonPublicAttributes(),
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testLessThanOrEqual() {
    $this->assertLessThanOrEqual(2, 1);

    try {
      $this->assertLessThanOrEqual(1, 2);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAttributeLessThanOrEqual() {
    $this->assertAttributeLessThanOrEqual(
      2,
      'foo',
      new ClassWithNonPublicAttributes(),
    );

    try {
      $this->assertAttributeLessThanOrEqual(
        1,
        'bar',
        new ClassWithNonPublicAttributes(),
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testReadAttribute() {
    $obj = new ClassWithNonPublicAttributes();

    $this->assertEquals('foo', $this->readAttribute($obj, 'publicAttribute'));
    $this->assertEquals(
      'bar',
      $this->readAttribute($obj, 'protectedAttribute'),
    );
    $this->assertEquals(
      'baz',
      $this->readAttribute($obj, 'privateAttribute'),
    );
    $this->assertEquals(
      'bar',
      $this->readAttribute($obj, 'protectedParentAttribute'),
    );
    //$this->assertEquals('bar', $this->readAttribute($obj, 'privateParentAttribute'));
  }

  public function testReadAttribute2() {
    $this->assertEquals(
      'foo',
      $this->readAttribute(
        ClassWithNonPublicAttributes::class,
        'publicStaticAttribute',
      ),
    );
    $this->assertEquals(
      'bar',
      $this->readAttribute(
        ClassWithNonPublicAttributes::class,
        'protectedStaticAttribute',
      ),
    );
    $this->assertEquals(
      'baz',
      $this->readAttribute(
        ClassWithNonPublicAttributes::class,
        'privateStaticAttribute',
      ),
    );
    $this->assertEquals(
      'foo',
      $this->readAttribute(
        ClassWithNonPublicAttributes::class,
        'protectedStaticParentAttribute',
      ),
    );
    $this->assertEquals(
      'foo',
      $this->readAttribute(
        ClassWithNonPublicAttributes::class,
        'privateStaticParentAttribute',
      ),
    );
  }

  public function testReadAttribute4() {
    try {
      $this->readAttribute('NotExistingClass', 'foo');
    } catch (InvalidArgumentException $e) {
      $this->assertTrue(true);
      return;
    }
    $this->fail();
  }

  public function testReadAttribute5() {
    try {
      $this->readAttribute(null, 'foo');
    } catch (InvalidArgumentException $e) {
      $this->assertTrue(true);
      return;
    }
    $this->fail();
  }

  public function testReadAttributeIfAttributeNameIsNotValid() {
    try {
      $this->readAttribute(stdClass::class, '2');
    } catch (InvalidArgumentException $e) {
      $this->assertTrue(true);
      return;
    }
    $this->fail();
  }

  public function testGetStaticAttributeRaisesExceptionForInvalidSecondArgument2(
  ) {
    try {
      $this->getStaticAttribute(stdClass::class, '0');
    } catch (InvalidArgumentException $e) {
      $this->assertTrue(true);
      return;
    }
    $this->fail();
  }

  public function testGetStaticAttributeRaisesExceptionForInvalidSecondArgument3(
  ) {
    try {
      $this->getStaticAttribute(stdClass::class, 'foo');
    } catch (AttributeNotFoundException $e) {
      $this->assertTrue(true);
      return;
    }
    $this->fail();
  }

  public function testGetObjectAttributeRaisesExceptionForInvalidFirstArgument(
  ) {
    try {
      $this->getObjectAttribute(null, 'foo');
    } catch (InvalidArgumentException $e) {
      $this->assertTrue(true);
      return;
    }
    $this->fail();
  }

  public function testGetObjectAttributeRaisesExceptionForInvalidSecondArgument2(
  ) {
    try {
      $this->getObjectAttribute(new stdClass(), '0');
    } catch (InvalidArgumentException $e) {
      $this->assertTrue(true);
      return;
    } catch (PHPUnit_Framework_Exception $e) {
      $this->assertTrue(true);
      return;
    }
    $this->fail();
  }

  public function testGetObjectAttributeRaisesExceptionForInvalidSecondArgument3(
  ) {
    try {
      $this->getObjectAttribute(new stdClass(), 'foo');
    } catch (AttributeNotFoundException $e) {
      $this->assertTrue(true);
      return true;
    } catch (PHPUnit_framework_Exception $e) {
      $this->assertTrue(true);
      return;
    }
    $this->fail();
  }

  public function testGetObjectAttributeWorksForInheritedAttributes() {
    $this->assertEquals(
      'bar',
      $this->getObjectAttribute(
        new ClassWithNonPublicAttributes(),
        'privateParentAttribute',
      ),
    );
  }

  public function testAssertPublicAttributeContains() {
    $obj = new ClassWithNonPublicAttributes();

    $this->assertAttributeContains('foo', 'publicArray', $obj);

    try {
      $this->assertAttributeContains('bar', 'publicArray', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail(
      'Failed exception trap, should of raised AssertionFailedException',
    );
  }

  public function testAssertPublicAttributeContainsOnly() {
    $obj = new ClassWithNonPublicAttributes();

    $this->assertAttributeContainsOnly('string', 'publicArray', $obj);

    try {
      $this->assertAttributeContainsOnly('integer', 'publicArray', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail(
      'Failed exception trap, should of raised AssertionFailedException',
    );

  }

  public function testAssertPublicAttributeNotContains() {
    $obj = new ClassWithNonPublicAttributes();

    $this->assertAttributeNotContains('bar', 'publicArray', $obj);

    try {
      $this->assertAttributeNotContains('foo', 'publicArray', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail(
      'Failed exception trap, should of raised AssertionFailedException',
    );
  }

  public function testAssertPublicAttributeNotContainsOnly() {
    $obj = new ClassWithNonPublicAttributes();

    $this->assertAttributeNotContainsOnly('integer', 'publicArray', $obj);

    try {
      $this->assertAttributeNotContainsOnly('string', 'publicArray', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertProtectedAttributeContains() {
    $obj = new ClassWithNonPublicAttributes();

    $this->assertAttributeContains('bar', 'protectedArray', $obj);

    try {
      $this->assertAttributeContains('foo', 'protectedArray', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertProtectedAttributeNotContains() {
    $obj = new ClassWithNonPublicAttributes();

    $this->assertAttributeNotContains('foo', 'protectedArray', $obj);

    try {
      $this->assertAttributeNotContains('bar', 'protectedArray', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertPrivateAttributeContains() {
    $obj = new ClassWithNonPublicAttributes();

    $this->assertAttributeContains('baz', 'privateArray', $obj);

    try {
      $this->assertAttributeContains('foo', 'privateArray', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertPrivateAttributeNotContains() {
    $obj = new ClassWithNonPublicAttributes();

    $this->assertAttributeNotContains('foo', 'privateArray', $obj);

    try {
      $this->assertAttributeNotContains('baz', 'privateArray', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertAttributeContainsNonObject() {
    $obj = new ClassWithNonPublicAttributes();

    $this->assertAttributeContains(true, 'privateArray', $obj);

    try {
      $this->assertAttributeContains(
        true,
        'privateArray',
        $obj,
        '',
        false,
        true,
        true,
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertAttributeNotContainsNonObject() {
    $obj = new ClassWithNonPublicAttributes();

    $this->assertAttributeNotContains(
      true,
      'privateArray',
      $obj,
      '',
      false,
      true,
      true,
    );

    try {
      $this->assertAttributeNotContains(true, 'privateArray', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertPublicAttributeEquals() {
    $obj = new ClassWithNonPublicAttributes();

    $this->assertAttributeEquals('foo', 'publicAttribute', $obj);

    try {
      $this->assertAttributeEquals('bar', 'publicAttribute', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertPublicAttributeNotEquals() {
    $obj = new ClassWithNonPublicAttributes();

    $this->assertAttributeNotEquals('bar', 'publicAttribute', $obj);

    try {
      $this->assertAttributeNotEquals('foo', 'publicAttribute', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertPublicAttributeSame() {
    $obj = new ClassWithNonPublicAttributes();

    $this->assertAttributeSame('foo', 'publicAttribute', $obj);

    try {
      $this->assertAttributeSame('bar', 'publicAttribute', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertPublicAttributeNotSame() {

    $obj = new ClassWithNonPublicAttributes();

    $this->assertAttributeNotSame('bar', 'publicAttribute', $obj);

    try {
      $this->assertAttributeNotSame('foo', 'publicAttribute', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertProtectedAttributeEquals() {
    $obj = new ClassWithNonPublicAttributes();

    $this->assertAttributeEquals('bar', 'protectedAttribute', $obj);

    try {
      $this->assertAttributeEquals('foo', 'protectedAttribute', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertProtectedAttributeNotEquals() {
    $obj = new ClassWithNonPublicAttributes();

    $this->assertAttributeNotEquals('foo', 'protectedAttribute', $obj);

    try {
      $this->assertAttributeNotEquals('bar', 'protectedAttribute', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertPrivateAttributeEquals() {
    $obj = new ClassWithNonPublicAttributes();

    $this->assertAttributeEquals('baz', 'privateAttribute', $obj);

    try {
      $this->assertAttributeEquals('foo', 'privateAttribute', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertPrivateAttributeNotEquals() {
    $obj = new ClassWithNonPublicAttributes();

    $this->assertAttributeNotEquals('foo', 'privateAttribute', $obj);

    try {
      $this->assertAttributeNotEquals('baz', 'privateAttribute', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertPublicStaticAttributeEquals() {
    $this->assertAttributeEquals(
      'foo',
      'publicStaticAttribute',
      ClassWithNonPublicAttributes::class,
    );

    try {
      $this->assertAttributeEquals(
        'bar',
        'publicStaticAttribute',
        ClassWithNonPublicAttributes::class,
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertPublicStaticAttributeNotEquals() {
    $this->assertAttributeNotEquals(
      'bar',
      'publicStaticAttribute',
      ClassWithNonPublicAttributes::class,
    );

    try {
      $this->assertAttributeNotEquals(
        'foo',
        'publicStaticAttribute',
        ClassWithNonPublicAttributes::class,
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertProtectedStaticAttributeEquals() {
    $this->assertAttributeEquals(
      'bar',
      'protectedStaticAttribute',
      ClassWithNonPublicAttributes::class,
    );

    try {
      $this->assertAttributeEquals(
        'foo',
        'protectedStaticAttribute',
        ClassWithNonPublicAttributes::class,
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertProtectedStaticAttributeNotEquals() {
    $this->assertAttributeNotEquals(
      'foo',
      'protectedStaticAttribute',
      ClassWithNonPublicAttributes::class,
    );

    try {
      $this->assertAttributeNotEquals(
        'bar',
        'protectedStaticAttribute',
        ClassWithNonPublicAttributes::class,
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertPrivateStaticAttributeEquals() {
    $this->assertAttributeEquals(
      'baz',
      'privateStaticAttribute',
      ClassWithNonPublicAttributes::class,
    );

    try {
      $this->assertAttributeEquals(
        'foo',
        'privateStaticAttribute',
        ClassWithNonPublicAttributes::class,
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertPrivateStaticAttributeNotEquals() {
    $this->assertAttributeNotEquals(
      'foo',
      'privateStaticAttribute',
      ClassWithNonPublicAttributes::class,
    );

    try {
      $this->assertAttributeNotEquals(
        'baz',
        'privateStaticAttribute',
        ClassWithNonPublicAttributes::class,
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertClassHasAttributeThrowsExceptionIfAttributeNameIsNotValid(
  ) {
    try {
      $this->assertClassHasAttribute('1', 'ClassWithNonPublicAttributes');
    } catch (InvalidArgumentException $e) {
      $this->assertTrue(true);
      return;
    } catch (PHPUnit_framework_Exception $e) {
      $this->assertTrue(true);
      return;
    }
    $this->fail();
  }

  public function testAssertClassNotHasAttributeThrowsExceptionIfAttributeNameIsNotValid(
  ) {
    try {
      $this->assertClassNotHasAttribute('1', 'ClassWithNonPublicAttributes');
    } catch (InvalidArgumentException $e) {
      $this->assertTrue(true);
      return;
    } catch (PHPUnit_framework_Exception $e) {
      $this->assertTrue(true);
      return;
    }
    $this->fail();
  }

  public function testAssertClassHasStaticAttributeThrowsExceptionIfAttributeNameIsNotValid(
  ) {
    try {
      $this->assertClassHasStaticAttribute(
        '1',
        'ClassWithNonPublicAttributes',
      );
    } catch (PHPUnit_framework_Exception $e) {
      $this->assertTrue(true);
      return;
    } catch (InvalidArgumentException $e) {
      $this->assertTrue(true);
      return;
    }
    $this->fail();
  }

  public function testAssertClassNotHasStaticAttributeThrowsExceptionIfAttributeNameIsNotValid(
  ) {
    try {
      $this->assertClassNotHasStaticAttribute(
        '1',
        'ClassWithNonPublicAttributes',
      );
    } catch (InvalidArgumentException $e) {
      $this->assertTrue(true);
      return;
    } catch (PHPUnit_Framework_Exception $e) {
      $this->assertTrue(true);
      return;
    }
    $this->fail();
  }

  public function testAssertObjectHasAttributeThrowsException2() {
    try {
      $this->assertObjectHasAttribute('foo', null);
    } catch (InvalidArgumentException $e) {
      $this->assertTrue(true);
      return;
    } catch (PHPUnit_Framework_Exception $e) {
      $this->assertTrue(true);
      return;
    }
    $this->fail();
  }

  public function testAssertObjectHasAttributeThrowsExceptionIfAttributeNameIsNotValid(
  ) {
    try {
      $this->assertObjectHasAttribute('1', 'ClassWithNonPublicAttributes');
    } catch (PHPUnit_Framework_Exception $e) {
      $this->assertTrue(true);
      return;
    } catch (InvalidArgumentException $e) {
      $this->assertTrue(true);
      return;
    }
    $this->fail();
  }

  public function testAssertObjectNotHasAttributeThrowsException2() {
    try {
      $this->assertObjectNotHasAttribute('foo', null);
    } catch (PHPUnit_Framework_Exception $e) {
      $this->assertTrue(true);
      return;
    } catch (InvalidArgumentException $e) {
      $this->assertTrue(true);
      return;
    }
  }

  public function testAssertObjectNotHasAttributeThrowsExceptionIfAttributeNameIsNotValid(
  ) {
    try {
      $this->assertObjectNotHasAttribute('1', 'ClassWithNonPublicAttributes');
    } catch (PHPUnit_Framework_Exception $e) {
      $this->assertTrue(true);
      return;
    } catch (InvalidArgumentException $e) {
      $this->assertTrue(true);
      return;
    }
    $this->fail();
  }

  public function testClassHasPublicAttribute() {
    $this->assertClassHasAttribute(
      'publicAttribute',
      ClassWithNonPublicAttributes::class,
    );

    try {
      $this->assertClassHasAttribute(
        'attribute',
        ClassWithNonPublicAttributes::class,
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testClassNotHasPublicAttribute() {
    $this->assertClassNotHasAttribute(
      'attribute',
      ClassWithNonPublicAttributes::class,
    );

    try {
      $this->assertClassNotHasAttribute(
        'publicAttribute',
        ClassWithNonPublicAttributes::class,
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testClassHasPublicStaticAttribute() {
    $this->assertClassHasStaticAttribute(
      'publicStaticAttribute',
      ClassWithNonPublicAttributes::class,
    );

    try {
      $this->assertClassHasStaticAttribute(
        'attribute',
        ClassWithNonPublicAttributes::class,
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testClassNotHasPublicStaticAttribute() {
    $this->assertClassNotHasStaticAttribute(
      'attribute',
      ClassWithNonPublicAttributes::class,
    );

    try {
      $this->assertClassNotHasStaticAttribute(
        'publicStaticAttribute',
        ClassWithNonPublicAttributes::class,
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testObjectHasPublicAttribute() {
    $obj = new ClassWithNonPublicAttributes();

    $this->assertObjectHasAttribute('publicAttribute', $obj);

    try {
      $this->assertObjectHasAttribute('attribute', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testObjectNotHasPublicAttribute() {
    $obj = new ClassWithNonPublicAttributes();

    $this->assertObjectNotHasAttribute('attribute', $obj);

    try {
      $this->assertObjectNotHasAttribute('publicAttribute', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testObjectHasOnTheFlyAttribute() {
    $obj = new stdClass();
    $obj->foo = 'bar';

    $this->assertObjectHasAttribute('foo', $obj);

    try {
      $this->assertObjectHasAttribute('bar', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testObjectNotHasOnTheFlyAttribute() {
    $obj = new stdClass();
    $obj->foo = 'bar';

    $this->assertObjectNotHasAttribute('bar', $obj);

    try {
      $this->assertObjectNotHasAttribute('foo', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testObjectHasProtectedAttribute() {
    $obj = new ClassWithNonPublicAttributes();

    $this->assertObjectHasAttribute('protectedAttribute', $obj);

    try {
      $this->assertObjectHasAttribute('attribute', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testObjectNotHasProtectedAttribute() {
    $obj = new ClassWithNonPublicAttributes();

    $this->assertObjectNotHasAttribute('attribute', $obj);

    try {
      $this->assertObjectNotHasAttribute('protectedAttribute', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testObjectHasPrivateAttribute() {
    $obj = new ClassWithNonPublicAttributes();

    $this->assertObjectHasAttribute('privateAttribute', $obj);

    try {
      $this->assertObjectHasAttribute('attribute', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testObjectNotHasPrivateAttribute() {
    $obj = new ClassWithNonPublicAttributes();

    $this->assertObjectNotHasAttribute('attribute', $obj);

    try {
      $this->assertObjectNotHasAttribute('privateAttribute', $obj);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertFileEquals() {
    $this->assertFileEquals(
      $this->getFilesDirectory().'foo.xml',
      $this->getFilesDirectory().'foo.xml',
    );

    try {
      $this->assertFileEquals(
        $this->getFilesDirectory().'foo.xml',
        $this->getFilesDirectory().'bar.xml',
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertFileNotEquals() {
    $this->assertFileNotEquals(
      $this->getFilesDirectory().'foo.xml',
      $this->getFilesDirectory().'bar.xml',
    );

    try {
      $this->assertFileNotEquals(
        $this->getFilesDirectory().'foo.xml',
        $this->getFilesDirectory().'foo.xml',
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertStringEqualsFile() {

    $this->assertStringEqualsFile(
      $this->getFilesDirectory().'foo.xml',
      file_get_contents($this->getFilesDirectory().'foo.xml'),
    );

    try {
      $this->assertStringEqualsFile(
        $this->getFilesDirectory().'foo.xml',
        file_get_contents($this->getFilesDirectory().'bar.xml'),
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail('Did not trap AssertionFailedException on differing files');
  }

  public function testAssertStringNotEqualsFile() {
    $this->assertStringNotEqualsFile(
      $this->getFilesDirectory().'foo.xml',
      file_get_contents($this->getFilesDirectory().'bar.xml'),
    );

    try {
      $this->assertStringNotEqualsFile(
        $this->getFilesDirectory().'foo.xml',
        file_get_contents($this->getFilesDirectory().'foo.xml'),
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertStringStartsWith() {
    $this->assertStringStartsWith('prefix', 'prefixfoo');

    try {
      $this->assertStringStartsWith('prefix', 'foo');
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertStringStartsNotWith() {
    $this->assertStringStartsNotWith('prefix', 'foo');

    try {
      $this->assertStringStartsNotWith('prefix', 'prefixfoo');
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertStringEndsWith() {
    $this->assertStringEndsWith('suffix', 'foosuffix');

    try {
      $this->assertStringEndsWith('suffix', 'foo');
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertStringEndsNotWith() {
    $this->assertStringEndsNotWith('suffix', 'foo');

    try {
      $this->assertStringEndsNotWith('suffix', 'foosuffix');
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertStringMatchesFormat() {
    $this->assertStringMatchesFormat('*%s*', '***');
  }

  public function testAssertStringMatchesFormatFailure() {
    try {
      $this->assertStringMatchesFormat('*%s*', '**');
    } catch (AssertionFailedException $e) {
      $this->assertTrue(true);
      return;
    }
  }

  public function testAssertStringNotMatchesFormat() {

    $this->assertStringNotMatchesFormat('*%s*', '**');

    try {
      $this->assertStringMatchesFormat('*%s*', '**');
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertEmpty() {
    $this->assertEmpty([]);

    try {
      $this->assertEmpty(['foo']);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertNotEmpty() {
    $this->assertNotEmpty(['foo']);

    try {
      $this->assertNotEmpty([]);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertAttributeEmpty() {
    $o = new stdClass();
    $o->a = [];

    $this->assertAttributeEmpty('a', $o);

    try {
      $o->a = ['b'];
      $this->assertAttributeEmpty('a', $o);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertAttributeNotEmpty() {
    $o = new stdClass();
    $o->a = ['b'];

    $this->assertAttributeNotEmpty('a', $o);

    try {
      $o->a = [];
      $this->assertAttributeNotEmpty('a', $o);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testMarkTestIncomplete() {

    try {
      $this->markTestIncomplete('incomplete');
    } catch (IncompleteException $e) {
      $this->assertEquals('incomplete', $e->getMessage());
      return;
    }

    $this->fail();
  }

  public function testMarkTestSkipped() {
    try {
      $this->markTestSkipped('skipped');
    } catch (SkippedException $e) {
      $this->assertEquals('skipped', $e->getMessage());
      return;
    }
    $this->fail();
  }

  public function testAssertCount() {
    $this->assertCount(2, [1, 2]);

    try {
      $this->assertCount(2, [1, 2, 3]);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertCountTraversable() {
    $this->assertCount(2, new ArrayIterator([1, 2]));

    try {
      $this->assertCount(2, new ArrayIterator([1, 2, 3]));
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  // JEO: invalid test function sig int, mixed, string
  // public function testAssertCountThrowsExceptionIfExpectedCountIsNoInteger()
  // {
  //     try {
  //         $this->assertCount('a', []);
  //     } catch (PHPUnit_Framework_Exception $e) {
  //         $this->assertEquals('Argument #1 (No Value) of PHPUnit_Framework_Assert::assertCount() must be a integer', $e->getMessage());
  //
  //         return;
  //     }
  //
  //     $this->fail();
  // }

  public function testAssertCountThrowsExceptionIfElementIsNotCountable() {
    try {
      $this->assertCount(2, '');
    } catch (InvalidArgumentException $e) {
      $this->assertEquals(
        'Argument #2 (No Value) of Zynga\PHPUnit\V2\Assertions\AssertCount::evaluate() must be a countable or traversable',
        $e->getMessage(),
      );

      return;
    } catch (PHPUnit_Framework_Exception $e) {
      $this->assertEquals(
        'Argument #2 (No Value) of PHPUnit_Framework_Assert::assertCount() must be a countable or traversable',
        $e->getMessage(),
      );

      return;
    }

    $this->fail();
  }

  public function testAssertAttributeCount() {
    $o = new stdClass();
    $o->a = [];

    $this->assertAttributeCount(0, 'a', $o);
  }

  public function testAssertNotCount() {
    $this->assertNotCount(2, [1, 2, 3]);

    try {
      $this->assertNotCount(2, [1, 2]);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertNotCountThrowsExceptionIfElementIsNotCountable() {

    try {
      $this->assertNotCount(2, '');
    } catch (InvalidArgumentException $e) {
      $this->assertTrue(true);
      return;
    } catch (PHPUnit_Framework_Exception $e) {
      $this->assertTrue(true);
      return;
    }
    $this->fail('Did not catch invalid argument expected exception.');

  }

  public function testAssertAttributeNotCount() {
    $o = new stdClass();
    $o->a = [];

    $this->assertAttributeNotCount(1, 'a', $o);
  }

  public function testAssertSameSize() {
    $this->assertSameSize([1, 2], [3, 4]);

    try {
      $this->assertSameSize([1, 2], [1, 2, 3]);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  // JEO: Invalid test function sig is array, array, string
  // public function testAssertSameSizeThrowsExceptionIfExpectedIsNotCountable()
  // {
  //     try {
  //         $this->assertSameSize('a', []);
  //     } catch (PHPUnit_Framework_Exception $e) {
  //         $this->assertEquals('Argument #1 (No Value) of PHPUnit_Framework_Assert::assertSameSize() must be a countable or traversable', $e->getMessage());
  //
  //         return;
  //     }
  //
  //     $this->fail();
  // }

  // JEO: invalid test function sig is array, array, string
  // public function testAssertSameSizeThrowsExceptionIfActualIsNotCountable()
  // {
  //     try {
  //         $this->assertSameSize([], '');
  //     } catch (PHPUnit_Framework_Exception $e) {
  //         $this->assertEquals('Argument #2 (No Value) of PHPUnit_Framework_Assert::assertSameSize() must be a countable or traversable', $e->getMessage());
  //
  //         return;
  //     }
  //
  //     $this->fail();
  // }

  public function testAssertNotSameSize() {
    $this->assertNotSameSize([1, 2], [1, 2, 3]);

    try {
      $this->assertNotSameSize([1, 2], [3, 4]);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertJson() {
    $this->assertJson('{}');
  }

  public function testAssertJsonStringEqualsJsonString() {
    $expected = '{"Mascott" : "Tux"}';
    $actual = '{"Mascott" : "Tux"}';
    $message = 'Given Json strings do not match';

    $this->assertJsonStringEqualsJsonString($expected, $actual, $message);
  }

  <<dataProvider("validInvalidJsonDataProvider")>>
  public function testAssertJsonStringEqualsJsonStringErrorRaised(
    string $expected,
    string $actual,
  ): void {

    try {
      $this->assertJsonStringEqualsJsonString($expected, $actual);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail('Expected exception not found');

  }

  public function testAssertJsonStringNotEqualsJsonString() {
    $expected = '{"Mascott" : "Beastie"}';
    $actual = '{"Mascott" : "Tux"}';
    $message = 'Given Json strings do match';

    $this->assertJsonStringNotEqualsJsonString($expected, $actual, $message);
  }

  <<dataProvider("validInvalidJsonDataProvider")>>
  public function testAssertJsonStringNotEqualsJsonStringErrorRaised(
    string $expected,
    string $actual,
  ): void {

    try {
      $this->assertJsonStringNotEqualsJsonString($expected, $actual);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail('Expected exception not found');

  }

  public function testAssertJsonStringEqualsJsonFile() {
    $file = $this->getFilesDirectory().'/JsonData/simpleObject.json';
    $actual = json_encode(['Mascott' => 'Tux']);
    $message = '';
    $this->assertJsonStringEqualsJsonFile($file, $actual, $message);
  }

  public function testAssertJsonStringEqualsJsonFileExpectingExpectationFailedException(
  ) {
    $file = $this->getFilesDirectory().'/JsonData/simpleObject.json';
    $actual = json_encode(['Mascott' => 'Beastie']);
    $message = '';
    try {
      $this->assertJsonStringEqualsJsonFile($file, $actual, $message);
    } catch (AssertionFailedException $e) {
      $this->assertEquals(
        'Failed asserting that \'{"Mascott":"Beastie"}\' matches JSON string "{"Mascott":"Tux"}".',
        $e->getMessage(),
      );

      return;
    } catch (PHPUnit_Framework_ExpectationFailedException $e) {
      $this->assertEquals(
        'Failed asserting that \'{"Mascott":"Beastie"}\' matches JSON string "{"Mascott":"Tux"}".',
        $e->getMessage(),
      );

      return;
    }

    $this->fail('Expected Exception not thrown.');
  }

  public function testAssertJsonStringEqualsJsonFileExpectingException() {
    $file = $this->getFilesDirectory().'/JsonData/simpleObject.json';
    try {
      $this->assertJsonStringEqualsJsonFile($file, '');
    } catch (AssertionFailedException $e) {
      return;
    } catch (PHPUnit_Framework_Exception $e) {
      return;
    }
    $this->fail('Expected Exception not thrown.');
  }

  public function testAssertJsonStringNotEqualsJsonFile() {
    $file = $this->getFilesDirectory().'/JsonData/simpleObject.json';
    $actual = json_encode(['Mascott' => 'Beastie']);
    $message = '';
    $this->assertJsonStringNotEqualsJsonFile($file, $actual, $message);
  }

  public function testAssertJsonStringNotEqualsJsonFileExpectingException() {
    $file = $this->getFilesDirectory().'/JsonData/simpleObject.json';
    try {
      $this->assertJsonStringNotEqualsJsonFile($file, '');
    } catch (AssertionFailedException $e) {
      return;
    } catch (PHPUnit_Framework_Exception $e) {
      return;
    }
    $this->fail('Expected exception not found.');
  }

  public function testAssertJsonFileNotEqualsJsonFile() {
    $fileExpected = $this->getFilesDirectory().'/JsonData/simpleObject.json';
    $fileActual = $this->getFilesDirectory().'/JsonData/arrayObject.json';
    $message = '';
    $this->assertJsonFileNotEqualsJsonFile(
      $fileExpected,
      $fileActual,
      $message,
    );
  }

  public function testAssertJsonFileEqualsJsonFile() {
    $file = $this->getFilesDirectory().'/JsonData/simpleObject.json';
    $message = '';
    $this->assertJsonFileEqualsJsonFile($file, $file, $message);
  }

  public function testAssertInstanceOf() {
    $this->assertInstanceOf(stdClass::class, new stdClass());

    try {
      $this->assertInstanceOf(Exception::class, new stdClass());
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertAttributeInstanceOf() {
    $o = new stdClass();
    $o->a = new stdClass();

    $this->assertAttributeInstanceOf(stdClass::class, 'a', $o);
  }

  public function testAssertNotInstanceOf() {
    $this->assertNotInstanceOf(Exception::class, new stdClass());

    try {
      $this->assertNotInstanceOf(stdClass::class, new stdClass());
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertAttributeNotInstanceOf() {
    $o = new stdClass();
    $o->a = new stdClass();

    // JEO: adjusted this from 'Exception', to Exception::class
    $this->assertAttributeNotInstanceOf(Exception::class, 'a', $o);
  }

  public function testAssertInternalType() {
    $this->assertInternalType('integer', 1);

    try {
      $this->assertInternalType('string', 1);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertInternalTypeDouble() {
    $this->assertInternalType('double', doubleval(1.0));

    try {
      $this->assertInternalType('double', 1);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertAttributeInternalType() {
    $o = new stdClass();
    $o->a = 1;

    $this->assertAttributeInternalType('integer', 'a', $o);
  }

  public function testAssertNotInternalType() {
    $this->assertNotInternalType('string', 1);

    try {
      $this->assertNotInternalType('integer', 1);
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertAttributeNotInternalType() {
    $o = new stdClass();
    $o->a = 1;

    $this->assertAttributeNotInternalType('string', 'a', $o);
  }

  public function testAssertStringMatchesFormatFileThrowsExceptionForInvalidArgument(
  ) {
    try {
      $this->assertStringMatchesFormatFile('not_existing_file', '');
    } catch (AssertionFailedException $e) {
      return;
    } catch (PHPUnit_Framework_Exception $e) {
      return;
    }
    $this->fail();
  }

  public function testAssertStringMatchesFormatFile() {
    $this->assertStringMatchesFormatFile(
      $this->getFilesDirectory().'expectedFileFormat.txt',
      "FOO\n",
    );

    try {
      $this->assertStringMatchesFormatFile(
        $this->getFilesDirectory().'expectedFileFormat.txt',
        "BAR\n",
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  public function testAssertStringNotMatchesFormatFileThrowsExceptionForInvalidArgument(
  ) {
    try {
      $this->assertStringNotMatchesFormatFile('not_existing_file', '');
    } catch (AssertionFailedException $e) {
      return;
    } catch (PHPUnit_Framework_Exception $e) {
      return;
    }
    $this->fail();
  }

  public function testAssertStringNotMatchesFormatFile() {
    $this->assertStringNotMatchesFormatFile(
      $this->getFilesDirectory().'expectedFileFormat.txt',
      "BAR\n",
    );

    try {
      $this->assertStringNotMatchesFormatFile(
        $this->getFilesDirectory().'expectedFileFormat.txt',
        "FOO\n",
      );
    } catch (AssertionFailedException $e) {
      return;
    }

    $this->fail();
  }

  // ------------------------------------------------------------------------------------------------------------------------------
  // Data providers for all of the assertions.
  // ------------------------------------------------------------------------------------------------------------------------------

  /**
   * @return array
   */
  public static function validInvalidJsonDataProvider() {
    return [
      'error syntax in expected JSON' => [
        '{"Mascott"::}',
        '{"Mascott" : "Tux"}',
      ],
      'error UTF-8 in actual JSON' => [
        '{"Mascott" : "Tux"}',
        '{"Mascott" : :}',
      ],
    ];
  }

  /**
   * @return array
   */
  public static function assertArraySubsetInvalidArgumentProvider() {
    return [[false, []], [[], false]];
  }

  public static function equalProvider() {
    // same |= equal
    return array_merge(self::equalValues(), self::sameValues());
  }

  public static function notSameProvider(): array<array<mixed, mixed>> {
    // not equal |= not same
    // equal, ¬same |= not same
    return array_merge(self::notEqualValues(), self::equalValues());
  }

  public static function sameProvider() {
    return self::sameValues();
  }

  public static function notEqualProvider() {
    return self::notEqualValues();
  }

  // ------------------------------------------------------------------------------------------------------------------------------
  // Data segements follow...
  // ------------------------------------------------------------------------------------------------------------------------------

  protected static string $testFilePath = '';

  protected static function getTestFileRoot(): string {

    if (self::$testFilePath == '') {
      self::$testFilePath =
        dirname(
          dirname(dirname(dirname(dirname(dirname(dirname(__FILE__)))))),
        ).
        '/tests/phpunit/_files';
    }

    return self::$testFilePath;

  }

  protected static function equalValues() {
    // cyclic dependencies
    $book1 = new Book();
    $book1->author = new Author('Terry Pratchett');
    $book1->author->books[] = $book1;
    $book2 = new Book();
    $book2->author = new Author('Terry Pratchett');
    $book2->author->books[] = $book2;

    $object1 = new SampleClass(4, 8, 15);
    $object2 = new SampleClass(4, 8, 15);
    $storage1 = new SplObjectStorage();
    $storage1->attach($object1);
    $storage2 = new SplObjectStorage();
    $storage2->attach($object1);

    return [
      // strings
      ['a', 'A', floatval(0), false, true], // ignore case
      // arrays
      [['a' => 1, 'b' => 2], ['b' => 2, 'a' => 1]],
      [[1], ['1']],
      [[3, 2, 1], [2, 3, 1], floatval(0), true], // canonicalized comparison
      // floats
      [2.3, 2.5, 0.5],
      [[2.3], [2.5], 0.5],
      [[[2.3]], [[2.5]], 0.5],
      [new Struct(2.3), new Struct(2.5), 0.5],
      [[new Struct(2.3)], [new Struct(2.5)], 0.5],
      // numeric with delta
      [1, 2, floatval(1)],
      // objects
      [$object1, $object2],
      [$book1, $book2],
      // SplObjectStorage
      [$storage1, $storage2],
      // DOMDocument
      [
        PHPUnit_Util_XML::load('<root></root>'),
        PHPUnit_Util_XML::load('<root/>'),
      ],
      [
        PHPUnit_Util_XML::load('<root attr="bar"></root>'),
        PHPUnit_Util_XML::load('<root attr="bar"/>'),
      ],
      [
        PHPUnit_Util_XML::load('<root><foo attr="bar"></foo></root>'),
        PHPUnit_Util_XML::load('<root><foo attr="bar"/></root>'),
      ],
      [
        PHPUnit_Util_XML::load("<root>\n  <child/>\n</root>"),
        PHPUnit_Util_XML::load('<root><child/></root>'),
      ],
      [
        new DateTime(
          '2013-03-29 04:13:35',
          new DateTimeZone('America/New_York'),
        ),
        new DateTime(
          '2013-03-29 04:13:35',
          new DateTimeZone('America/New_York'),
        ),
      ],
      [
        new DateTime(
          '2013-03-29 04:13:35',
          new DateTimeZone('America/New_York'),
        ),
        new DateTime(
          '2013-03-29 04:13:25',
          new DateTimeZone('America/New_York'),
        ),
        floatval(10),
      ],
      [
        new DateTime(
          '2013-03-29 04:13:35',
          new DateTimeZone('America/New_York'),
        ),
        new DateTime(
          '2013-03-29 04:14:40',
          new DateTimeZone('America/New_York'),
        ),
        floatval(65),
      ],
      [
        new DateTime('2013-03-29', new DateTimeZone('America/New_York')),
        new DateTime('2013-03-29', new DateTimeZone('America/New_York')),
      ],
      [
        new DateTime(
          '2013-03-29 04:13:35',
          new DateTimeZone('America/New_York'),
        ),
        new DateTime(
          '2013-03-29 03:13:35',
          new DateTimeZone('America/Chicago'),
        ),
      ],
      [
        new DateTime(
          '2013-03-29 04:13:35',
          new DateTimeZone('America/New_York'),
        ),
        new DateTime(
          '2013-03-29 03:13:49',
          new DateTimeZone('America/Chicago'),
        ),
        floatval(15),
      ],
      [
        new DateTime('2013-03-30', new DateTimeZone('America/New_York')),
        new DateTime(
          '2013-03-29 23:00:00',
          new DateTimeZone('America/Chicago'),
        ),
      ],
      [
        new DateTime('2013-03-30', new DateTimeZone('America/New_York')),
        new DateTime(
          '2013-03-29 23:01:30',
          new DateTimeZone('America/Chicago'),
        ),
        floatval(100),
      ],
      [
        new DateTime('@1364616000'),
        new DateTime(
          '2013-03-29 23:00:00',
          new DateTimeZone('America/Chicago'),
        ),
      ],
      [
        new DateTime('2013-03-29T05:13:35-0500'),
        new DateTime('2013-03-29T04:13:35-0600'),
      ],
      // Exception
      //array(new Exception('Exception 1'), new Exception('Exception 1')),
      // mixed types
      [0, '0'],
      ['0', 0],
      [2.3, '2.3'],
      ['2.3', 2.3],
      [(string) (1 / 3), 1 - 2 / 3],
      [1 / 3, (string) (1 - 2 / 3)],
      ['string representation', new ClassWithToString()],
      [new ClassWithToString(), 'string representation'],
    ];
  }

  protected static function sameValues() {

    $object = new SampleClass(4, 8, 15);
    // cannot use $filesDirectory, because neither setUp() nor
    // setUpBeforeClass() are executed before the data providers
    $file = self::getTestFileRoot().DIRECTORY_SEPARATOR.'foo.xml';
    $resource = fopen($file, 'r');

    return [
      // null
      [null, null],
      // strings
      ['a', 'a'],
      // integers
      [0, 0],
      // floats
      [2.3, 2.3],
      [1 / 3, 1 - 2 / 3],
      [log(0), log(0)],
      // arrays
      [[], []],
      [[0 => 1], [0 => 1]],
      [[0 => null], [0 => null]],
      // JEO: type checker does not let you make an array that mixes use
      // for array contexts
      // [['a', 'b' => [1, 2]], ['a', 'b' => [1, 2]]],
      // objects
      [$object, $object],
      // resources
      [$resource, $resource],
    ];
  }

  protected static function notEqualValues() {
    // cyclic dependencies
    $book1 = new Book();
    $book1->author = new Author('Terry Pratchett');
    $book1->author->books[] = $book1;
    $book2 = new Book();
    $book2->author = new Author('Terry Pratch');
    $book2->author->books[] = $book2;

    $book3 = new Book();
    $book3->author = new Author('Terry Pratchett');
    $book4 = new stdClass();
    $book4->author = new Author('Terry Pratchett');

    $object1 = new SampleClass(4, 8, 15);
    $object2 = new SampleClass(16, 23, 42);
    $object3 = new SampleClass(4, 8, 15);
    $storage1 = new SplObjectStorage();
    $storage1->attach($object1);
    $storage2 = new SplObjectStorage();
    $storage2->attach($object3); // same content, different object

    // cannot use $filesDirectory, because neither setUp() nor
    // setUpBeforeClass() are executed before the data providers
    $file = self::getTestFileRoot().DIRECTORY_SEPARATOR.'foo.xml';

    return [
      // strings
      ['a', 'b'],
      ['a', 'A'],
      // https://github.com/sebastianbergmann/phpunit/issues/1023
      ['9E6666666', '9E7777777'],
      // integers
      [1, 2],
      [2, 1],
      // floats
      [2.3, 4.2],
      [2.3, 4.2, 0.5],
      [[2.3], [4.2], 0.5],
      [[[2.3]], [[4.2]], 0.5],
      [new Struct(2.3), new Struct(4.2), 0.5],
      [[new Struct(2.3)], [new Struct(4.2)], 0.5],
      // NAN
      [NAN, NAN],
      // arrays
      [[], [0 => 1]],
      [[0 => 1], []],
      [[0 => null], []],
      [[0 => 1, 1 => 2], [0 => 1, 1 => 3]],
      // Type checker does not let you make array that mixes context.
      // [['a', 'b' => [1, 2]], ['a', 'b' => [2, 1]]],
      // objects
      [new SampleClass(4, 8, 15), new SampleClass(16, 23, 42)],
      [$object1, $object2],
      [$book1, $book2],
      [$book3, $book4], // same content, different class
      // resources
      [fopen($file, 'r'), fopen($file, 'r')],
      // SplObjectStorage
      [$storage1, $storage2],
      // DOMDocument
      [
        PHPUnit_Util_XML::load('<root></root>'),
        PHPUnit_Util_XML::load('<bar/>'),
      ],
      [
        PHPUnit_Util_XML::load('<foo attr1="bar"/>'),
        PHPUnit_Util_XML::load('<foo attr1="foobar"/>'),
      ],
      [
        PHPUnit_Util_XML::load('<foo> bar </foo>'),
        PHPUnit_Util_XML::load('<foo />'),
      ],
      [
        PHPUnit_Util_XML::load('<foo xmlns="urn:myns:bar"/>'),
        PHPUnit_Util_XML::load('<foo xmlns="urn:notmyns:bar"/>'),
      ],
      [
        PHPUnit_Util_XML::load('<foo> bar </foo>'),
        PHPUnit_Util_XML::load('<foo> bir </foo>'),
      ],
      [
        new DateTime(
          '2013-03-29 04:13:35',
          new DateTimeZone('America/New_York'),
        ),
        new DateTime(
          '2013-03-29 03:13:35',
          new DateTimeZone('America/New_York'),
        ),
      ],
      [
        new DateTime(
          '2013-03-29 04:13:35',
          new DateTimeZone('America/New_York'),
        ),
        new DateTime(
          '2013-03-29 03:13:35',
          new DateTimeZone('America/New_York'),
        ),
        floatval(3500),
      ],
      [
        new DateTime(
          '2013-03-29 04:13:35',
          new DateTimeZone('America/New_York'),
        ),
        new DateTime(
          '2013-03-29 05:13:35',
          new DateTimeZone('America/New_York'),
        ),
        floatval(3500),
      ],
      [
        new DateTime('2013-03-29', new DateTimeZone('America/New_York')),
        new DateTime('2013-03-30', new DateTimeZone('America/New_York')),
      ],
      [
        new DateTime('2013-03-29', new DateTimeZone('America/New_York')),
        new DateTime('2013-03-30', new DateTimeZone('America/New_York')),
        floatval(43200),
      ],
      [
        new DateTime(
          '2013-03-29 04:13:35',
          new DateTimeZone('America/New_York'),
        ),
        new DateTime(
          '2013-03-29 04:13:35',
          new DateTimeZone('America/Chicago'),
        ),
      ],
      [
        new DateTime(
          '2013-03-29 04:13:35',
          new DateTimeZone('America/New_York'),
        ),
        new DateTime(
          '2013-03-29 04:13:35',
          new DateTimeZone('America/Chicago'),
        ),
        floatval(3500),
      ],
      [
        new DateTime('2013-03-30', new DateTimeZone('America/New_York')),
        new DateTime('2013-03-30', new DateTimeZone('America/Chicago')),
      ],
      [
        new DateTime('2013-03-29T05:13:35-0600'),
        new DateTime('2013-03-29T04:13:35-0600'),
      ],
      [
        new DateTime('2013-03-29T05:13:35-0600'),
        new DateTime('2013-03-29T05:13:35-0500'),
      ],
      // Exception
      //array(new Exception('Exception 1'), new Exception('Exception 2')),
      // different types
      [new SampleClass(4, 8, 15), false],
      [false, new SampleClass(4, 8, 15)],
      [[0 => 1, 1 => 2], false],
      [false, [0 => 1, 1 => 2]],
      [[], new stdClass()],
      [new stdClass(), []],
      // PHP: 0 == 'Foobar' => true!
      // We want these values to differ
      [0, 'Foobar'],
      ['Foobar', 0],
      [3, acos(8)],
      [acos(8), 3],
    ];
  }

}
