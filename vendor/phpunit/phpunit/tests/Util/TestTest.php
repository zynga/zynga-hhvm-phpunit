<?php
/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

if (!defined('TEST_FILES_PATH')) {
    define(
        'TEST_FILES_PATH',
        dirname(__DIR__) . DIRECTORY_SEPARATOR .
        '_files' . DIRECTORY_SEPARATOR
    );
}

require TEST_FILES_PATH . 'NamespaceCoveredFunction.php';

use PHPUnit\Exceptions\CodeCoverageException;
use PHPUnit\Exceptions\Exception as PHPUnit_Exceptions_Exception;
use PHPUnit\Util\UtilTest;

/**
 * @since      Class available since Release 3.3.6
 */
class Util_TestTest extends PHPUnit_Framework_TestCase
{
    /**
     * @covers UtilTest::getExpectedException
     *
     * @todo   Split up in separate tests
     */
    public function testGetExpectedException()
    {
        $this->assertArraySubset(
          ['class' => 'FooBarBaz', 'code' => null, 'message' => ''],
          UtilTest::getExpectedException('ExceptionTest', 'testOne')
        );

        $this->assertArraySubset(
          ['class' => 'Foo_Bar_Baz', 'code' => null, 'message' => ''],
          UtilTest::getExpectedException('ExceptionTest', 'testTwo')
        );

        $this->assertArraySubset(
          ['class' => 'Foo\Bar\Baz', 'code' => null, 'message' => ''],
          UtilTest::getExpectedException('ExceptionTest', 'testThree')
        );

        $this->assertArraySubset(
          ['class' => 'ほげ', 'code' => null, 'message' => ''],
          UtilTest::getExpectedException('ExceptionTest', 'testFour')
        );

        $this->assertArraySubset(
          ['class' => 'Class', 'code' => 1234, 'message' => 'Message'],
          UtilTest::getExpectedException('ExceptionTest', 'testFive')
        );

        $this->assertArraySubset(
          ['class' => 'Class', 'code' => 1234, 'message' => 'Message'],
          UtilTest::getExpectedException('ExceptionTest', 'testSix')
        );

        $this->assertArraySubset(
          ['class' => 'Class', 'code' => 'ExceptionCode', 'message' => 'Message'],
          UtilTest::getExpectedException('ExceptionTest', 'testSeven')
        );

        $this->assertArraySubset(
          ['class' => 'Class', 'code' => 0, 'message' => 'Message'],
          UtilTest::getExpectedException('ExceptionTest', 'testEight')
        );

        $this->assertArraySubset(
          ['class' => 'Class', 'code' => ExceptionTest::ERROR_CODE, 'message' => ExceptionTest::ERROR_MESSAGE],
          UtilTest::getExpectedException('ExceptionTest', 'testNine')
        );

        $this->assertArraySubset(
          ['class' => 'Class', 'code' => null, 'message' => ''],
          UtilTest::getExpectedException('ExceptionTest', 'testSingleLine')
        );

        $this->assertArraySubset(
          ['class' => 'Class', 'code' => My\Space\ExceptionNamespaceTest::ERROR_CODE, 'message' => My\Space\ExceptionNamespaceTest::ERROR_MESSAGE],
          UtilTest::getExpectedException('My\Space\ExceptionNamespaceTest', 'testConstants')
        );

        // Ensure the Class::CONST expression is only evaluated when the constant really exists
        $this->assertArraySubset(
          ['class' => 'Class', 'code' => 'ExceptionTest::UNKNOWN_CODE_CONSTANT', 'message' => 'ExceptionTest::UNKNOWN_MESSAGE_CONSTANT'],
          UtilTest::getExpectedException('ExceptionTest', 'testUnknownConstants')
        );

        $this->assertArraySubset(
          ['class' => 'Class', 'code' => 'My\Space\ExceptionNamespaceTest::UNKNOWN_CODE_CONSTANT', 'message' => 'My\Space\ExceptionNamespaceTest::UNKNOWN_MESSAGE_CONSTANT'],
          UtilTest::getExpectedException('My\Space\ExceptionNamespaceTest', 'testUnknownConstants')
        );
    }

    /**
     * @covers UtilTest::getExpectedException
     */
    public function testGetExpectedRegExp()
    {
        $this->assertArraySubset(
          ['message_regex' => '#regex#'],
          UtilTest::getExpectedException('ExceptionTest', 'testWithRegexMessage')
        );

        $this->assertArraySubset(
          ['message_regex' => '#regex#'],
          UtilTest::getExpectedException('ExceptionTest', 'testWithRegexMessageFromClassConstant')
        );

        $this->assertArraySubset(
          ['message_regex' => 'ExceptionTest::UNKNOWN_MESSAGE_REGEX_CONSTANT'],
          UtilTest::getExpectedException('ExceptionTest', 'testWithUnknowRegexMessageFromClassConstant')
        );
    }

    /**
     * @covers       UtilTest::getRequirements
     * @dataProvider requirementsProvider
     */
    public function testGetRequirements($test, $result)
    {
        $this->assertEquals(
            $result,
            UtilTest::getRequirements('RequirementsTest', $test)
        );
    }

    public function requirementsProvider()
    {
        return [
            ['testOne',    []],
            ['testTwo',    ['PHPUnit'    => ['version' => '1.0', 'operator' => '']]],
            ['testThree',  ['PHP'        => ['version' => '2.0', 'operator' => '']]],
            ['testFour',   [
                'PHPUnit'    => ['version' => '2.0', 'operator' => ''],
                'PHP'        => ['version' => '1.0', 'operator' => '']]
            ],
            ['testFive',   ['PHP'        => ['version' => '5.4.0RC6', 'operator' => '']]],
            ['testSix',    ['PHP'        => ['version' => '5.4.0-alpha1', 'operator' => '']]],
            ['testSeven',  ['PHP'        => ['version' => '5.4.0beta2', 'operator' => '']]],
            ['testEight',  ['PHP'        => ['version' => '5.4-dev', 'operator' => '']]],
            ['testNine',   ['functions'  => ['testFunc']]],
            ['testTen',    ['extensions' => ['testExt']]],
            ['testEleven', ['OS'         => '/Linux/i']],
            [
              'testSpace',
              [
                'extensions' => ['spl'],
                'OS'         => '/.*/i'
              ]
            ],
            [
              'testAllPossibleRequirements',
              [
                'PHP'       => ['version' => '99-dev', 'operator' => ''],
                'PHPUnit'   => ['version' => '9-dev', 'operator' => ''],
                'OS'        => '/DOESNOTEXIST/i',
                'functions' => [
                  'testFuncOne',
                  'testFuncTwo',
                ],
                'extensions' => [
                  'testExtOne',
                  'testExtTwo',
                  'testExtThree',
                ],
                'extension_versions' => [
                    'testExtThree' => ['version' => '2.0', 'operator' => '']
                ]
              ]
            ],
            ['testSpecificExtensionVersion',
                [
                    'extension_versions' => ['testExt' => ['version' => '1.8.0', 'operator' => '']],
                    'extensions'         => ['testExt']
                ]
            ],
            ['testPHPVersionOperatorLessThan',
                [
                    'PHP' => ['version' => '5.4', 'operator' => '<']
                ]
            ],
            ['testPHPVersionOperatorLessThanEquals',
                [
                    'PHP' => ['version' => '5.4', 'operator' => '<=']
                ]
            ],
            ['testPHPVersionOperatorGreaterThan',
                [
                    'PHP' => ['version' => '99', 'operator' => '>']
                ]
            ],
            ['testPHPVersionOperatorGreaterThanEquals',
                [
                    'PHP' => ['version' => '99', 'operator' => '>=']
                ]
            ],
            ['testPHPVersionOperatorEquals',
                [
                    'PHP' => ['version' => '5.4', 'operator' => '=']
                ]
            ],
            ['testPHPVersionOperatorDoubleEquals',
                [
                    'PHP' => ['version' => '5.4', 'operator' => '==']
                ]
            ],
            ['testPHPVersionOperatorBangEquals',
                [
                    'PHP' => ['version' => '99', 'operator' => '!=']
                ]
            ],
            ['testPHPVersionOperatorNotEquals',
                [
                    'PHP' => ['version' => '99', 'operator' => '<>']
                ]
            ],
            ['testPHPVersionOperatorNoSpace',
                [
                    'PHP' => ['version' => '99', 'operator' => '>=']
                ]
            ],
            ['testPHPUnitVersionOperatorLessThan',
                [
                    'PHPUnit' => ['version' => '1.0', 'operator' => '<']
                ]
            ],
            ['testPHPUnitVersionOperatorLessThanEquals',
                [
                    'PHPUnit' => ['version' => '1.0', 'operator' => '<=']
                ]
            ],
            ['testPHPUnitVersionOperatorGreaterThan',
                [
                    'PHPUnit' => ['version' => '99', 'operator' => '>']
                ]
            ],
            ['testPHPUnitVersionOperatorGreaterThanEquals',
                [
                    'PHPUnit' => ['version' => '99', 'operator' => '>=']
                ]
            ],
            ['testPHPUnitVersionOperatorEquals',
                [
                    'PHPUnit' => ['version' => '1.0', 'operator' => '=']
                ]
            ],
            ['testPHPUnitVersionOperatorDoubleEquals',
                [
                    'PHPUnit' => ['version' => '1.0', 'operator' => '==']
                ]
            ],
            ['testPHPUnitVersionOperatorBangEquals',
                [
                    'PHPUnit' => ['version' => '99', 'operator' => '!=']
                ]
            ],
            ['testPHPUnitVersionOperatorNotEquals',
                [
                    'PHPUnit' => ['version' => '99', 'operator' => '<>']
                ]
            ],
            ['testPHPUnitVersionOperatorNoSpace',
                [
                    'PHPUnit' => ['version' => '99', 'operator' => '>=']
                ]
            ],
            ['testExtensionVersionOperatorLessThanEquals',
                [
                    'extensions'         => ['testExtOne'],
                    'extension_versions' => ['testExtOne' => ['version' => '1.0', 'operator' => '<=']]
                ]
            ],
            ['testExtensionVersionOperatorGreaterThan',
                [
                    'extensions'         => ['testExtOne'],
                    'extension_versions' => ['testExtOne' => ['version' => '99', 'operator' => '>']]
                ]
            ],
            ['testExtensionVersionOperatorGreaterThanEquals',
                [
                    'extensions'         => ['testExtOne'],
                    'extension_versions' => ['testExtOne' => ['version' => '99', 'operator' => '>=']]
                ]
            ],
            ['testExtensionVersionOperatorEquals',
                [
                    'extensions'         => ['testExtOne'],
                    'extension_versions' => ['testExtOne' => ['version' => '1.0', 'operator' => '=']]
                ]
            ],
            ['testExtensionVersionOperatorDoubleEquals',
                [
                    'extensions'         => ['testExtOne'],
                    'extension_versions' => ['testExtOne' => ['version' => '1.0', 'operator' => '==']]
                ]
            ],
            ['testExtensionVersionOperatorBangEquals',
                [
                    'extensions'         => ['testExtOne'],
                    'extension_versions' => ['testExtOne' => ['version' => '99', 'operator' => '!=']]
                ]
            ],
            ['testExtensionVersionOperatorNotEquals',
                [
                    'extensions'         => ['testExtOne'],
                    'extension_versions' => ['testExtOne' => ['version' => '99', 'operator' => '<>']]
                ]
            ],
            ['testExtensionVersionOperatorNoSpace',
                [
                    'extensions'         => ['testExtOne'],
                    'extension_versions' => ['testExtOne' => ['version' => '99', 'operator' => '>=']]
                ]
            ],
        ];
    }

    /**
     * @covers UtilTest::getRequirements
     */
    public function testGetRequirementsMergesClassAndMethodDocBlocks()
    {
        $expectedAnnotations = [
            'PHP'       => ['version' => '5.4', 'operator' => ''],
            'PHPUnit'   => ['version' => '3.7', 'operator' => ''],
            'OS'        => '/WINNT/i',
            'functions' => [
              'testFuncClass',
              'testFuncMethod',
            ],
            'extensions' => [
              'testExtClass',
              'testExtMethod',
            ]
        ];

        $this->assertEquals(
            $expectedAnnotations,
            UtilTest::getRequirements('RequirementsClassDocBlockTest', 'testMethod')
        );
    }

    /**
     * @covers       UtilTest::getMissingRequirements
     * @dataProvider missingRequirementsProvider
     */
    public function testGetMissingRequirements($test, $result)
    {
        $this->assertEquals(
            $result,
            UtilTest::getMissingRequirements('RequirementsTest', $test)
        );
    }

    public function missingRequirementsProvider()
    {
        return [
            ['testOne',            []],
            ['testNine',           ['Function testFunc is required.']],
            ['testTen',            ['Extension testExt is required.']],
            ['testAlwaysSkip',     ['PHPUnit >= 1111111 is required.']],
            ['testAlwaysSkip2',    ['PHP >= 9999999 is required.']],
            ['testAlwaysSkip3',    ['Operating system matching /DOESNOTEXIST/i is required.']],
            ['testAllPossibleRequirements', [
              'PHP >= 99-dev is required.',
              'PHPUnit >= 9-dev is required.',
              'Operating system matching /DOESNOTEXIST/i is required.',
              'Function testFuncOne is required.',
              'Function testFuncTwo is required.',
              'Extension testExtOne is required.',
              'Extension testExtTwo is required.',
              'Extension testExtThree >= 2.0 is required.',
            ]],
            ['testPHPVersionOperatorLessThan', ['PHP < 5.4 is required.']],
            ['testPHPVersionOperatorLessThanEquals', ['PHP <= 5.4 is required.']],
            ['testPHPVersionOperatorGreaterThan', ['PHP > 99 is required.']],
            ['testPHPVersionOperatorGreaterThanEquals', ['PHP >= 99 is required.']],
            ['testPHPVersionOperatorNoSpace', ['PHP >= 99 is required.']],
            ['testPHPVersionOperatorEquals', ['PHP = 5.4 is required.']],
            ['testPHPVersionOperatorDoubleEquals', ['PHP == 5.4 is required.']],
            ['testPHPUnitVersionOperatorLessThan', ['PHPUnit < 1.0 is required.']],
            ['testPHPUnitVersionOperatorLessThanEquals', ['PHPUnit <= 1.0 is required.']],
            ['testPHPUnitVersionOperatorGreaterThan', ['PHPUnit > 99 is required.']],
            ['testPHPUnitVersionOperatorGreaterThanEquals', ['PHPUnit >= 99 is required.']],
            ['testPHPUnitVersionOperatorEquals', ['PHPUnit = 1.0 is required.']],
            ['testPHPUnitVersionOperatorDoubleEquals', ['PHPUnit == 1.0 is required.']],
            ['testPHPUnitVersionOperatorNoSpace', ['PHPUnit >= 99 is required.']],
            ['testExtensionVersionOperatorLessThan', ['Extension testExtOne < 1.0 is required.']],
            ['testExtensionVersionOperatorLessThanEquals', ['Extension testExtOne <= 1.0 is required.']],
            ['testExtensionVersionOperatorGreaterThan', ['Extension testExtOne > 99 is required.']],
            ['testExtensionVersionOperatorGreaterThanEquals', ['Extension testExtOne >= 99 is required.']],
            ['testExtensionVersionOperatorEquals', ['Extension testExtOne = 1.0 is required.']],
            ['testExtensionVersionOperatorDoubleEquals', ['Extension testExtOne == 1.0 is required.']],
            ['testExtensionVersionOperatorNoSpace', ['Extension testExtOne >= 99 is required.']],
        ];
    }

    /**
     * @coversNothing
     *
     * @todo   This test does not really test functionality of PHPUnit_Util_Test
     */
    public function testGetProvidedDataRegEx()
    {
        $result = preg_match(UtilTest::REGEX_DATA_PROVIDER, '@dataProvider method', $matches);
        $this->assertEquals(1, $result);
        $this->assertEquals('method', $matches[1]);

        $result = preg_match(UtilTest::REGEX_DATA_PROVIDER, '@dataProvider class::method', $matches);
        $this->assertEquals(1, $result);
        $this->assertEquals('class::method', $matches[1]);

        $result = preg_match(UtilTest::REGEX_DATA_PROVIDER, '@dataProvider namespace\class::method', $matches);
        $this->assertEquals(1, $result);
        $this->assertEquals('namespace\class::method', $matches[1]);

        $result = preg_match(UtilTest::REGEX_DATA_PROVIDER, '@dataProvider namespace\namespace\class::method', $matches);
        $this->assertEquals(1, $result);
        $this->assertEquals('namespace\namespace\class::method', $matches[1]);

        $result = preg_match(UtilTest::REGEX_DATA_PROVIDER, '@dataProvider メソッド', $matches);
        $this->assertEquals(1, $result);
        $this->assertEquals('メソッド', $matches[1]);
    }

    /**
     * @covers UtilTest::getDataFromTestWithAnnotation
     */
    public function testTestWithEmptyAnnotation()
    {
        $result = UtilTest::getDataFromTestWithAnnotation("/**\n * @anotherAnnotation\n */");
        $this->assertNull($result);
    }

    /**
     * @covers UtilTest::getDataFromTestWithAnnotation
     */
    public function testTestWithSimpleCase()
    {
        $result = UtilTest::getDataFromTestWithAnnotation('/**
                                                                     * @testWith [1]
                                                                     */');
        $this->assertEquals([[1]], $result);
    }

    /**
     * @covers UtilTest::getDataFromTestWithAnnotation
     */
    public function testTestWithMultiLineMultiParameterCase()
    {
        $result = UtilTest::getDataFromTestWithAnnotation('/**
                                                                     * @testWith [1, 2]
                                                                     * [3, 4]
                                                                     */');
        $this->assertEquals([[1, 2], [3, 4]], $result);
    }

    /**
     * @covers UtilTest::getDataFromTestWithAnnotation
     */
    public function testTestWithVariousTypes()
    {
        $result = UtilTest::getDataFromTestWithAnnotation('/**
            * @testWith ["ab"]
            *           [true]
            *           [null]
         */');
        $this->assertEquals([['ab'], [true], [null]], $result);
    }

    /**
     * @covers UtilTest::getDataFromTestWithAnnotation
     */
    public function testTestWithAnnotationAfter()
    {
        $result = UtilTest::getDataFromTestWithAnnotation('/**
                                                                     * @testWith [1]
                                                                     *           [2]
                                                                     * @annotation
                                                                     */');
        $this->assertEquals([[1], [2]], $result);
    }

    /**
     * @covers UtilTest::getDataFromTestWithAnnotation
     */
    public function testTestWithSimpleTextAfter()
    {
        $result = UtilTest::getDataFromTestWithAnnotation('/**
                                                                     * @testWith [1]
                                                                     *           [2]
                                                                     * blah blah
                                                                     */');
        $this->assertEquals([[1], [2]], $result);
    }

    /**
     * @covers UtilTest::getDataFromTestWithAnnotation
     */
    public function testTestWithCharacterEscape()
    {
        $result = UtilTest::getDataFromTestWithAnnotation('/**
                                                                     * @testWith ["\"", "\""]
                                                                     */');
        $this->assertEquals([['"', '"']], $result);
    }

    /**
     * @covers UtilTest::getDataFromTestWithAnnotation
     */
    public function testTestWithThrowsProperExceptionIfDatasetCannotBeParsed()
    {
        $this->setExpectedExceptionRegExp(
            'PHPUnit_Exceptions_Exception',
            '/^The dataset for the @testWith annotation cannot be parsed.$/'
        );
        UtilTest::getDataFromTestWithAnnotation('/**
                                                           * @testWith [s]
                                                           */');
    }

    /**
     * @covers UtilTest::getDependencies
     *
     * @todo   Not sure what this test tests (name is misleading at least)
     */
    public function testParseAnnotation()
    {
        $this->assertEquals(
            ['Foo', 'ほげ'],
            UtilTest::getDependencies(get_class($this), 'methodForTestParseAnnotation')
        );
    }

    /**
     * @depends Foo
     * @depends ほげ
     *
     * @todo    Remove fixture from test class
     */
    public function methodForTestParseAnnotation()
    {
    }

    /**
     * @covers UtilTest::getDependencies
     */
    public function testParseAnnotationThatIsOnlyOneLine()
    {
        $this->assertEquals(
            ['Bar'],
            UtilTest::getDependencies(get_class($this), 'methodForTestParseAnnotationThatIsOnlyOneLine')
        );
    }

    /** @depends Bar */
    public function methodForTestParseAnnotationThatIsOnlyOneLine()
    {
        // TODO Remove fixture from test class
    }

    /**
     * @covers       UtilTest::getLinesToBeCovered
     * @covers       UtilTest::getLinesToBeCoveredOrUsed
     * @covers       UtilTest::resolveElementToReflectionObjects
     * @dataProvider getLinesToBeCoveredProvider
     */
    public function testGetLinesToBeCovered($test, $lines)
    {
        if (strpos($test, 'Namespace') === 0) {
            $expected = [
              TEST_FILES_PATH . 'NamespaceCoveredClass.php' => $lines
            ];
        } elseif ($test === 'CoverageNoneTest') {
            $expected = [];
        } elseif ($test === 'CoverageNothingTest') {
            $expected = false;
        } elseif ($test === 'CoverageFunctionTest') {
            $expected = [
              TEST_FILES_PATH . 'CoveredFunction.php' => $lines
            ];
        } else {
            $expected = [TEST_FILES_PATH . 'CoveredClass.php' => $lines];
        }

        $this->assertEquals(
            $expected,
            UtilTest::getLinesToBeCovered(
                $test, 'testSomething'
            )
        );
    }

    /**
     * @covers            UtilTest::getLinesToBeCovered
     * @covers            UtilTest::getLinesToBeCoveredOrUsed
     * @covers            UtilTest::resolveElementToReflectionObjects
     * @expectedException CodeCoverageException
     */
    public function testGetLinesToBeCovered2()
    {
        UtilTest::getLinesToBeCovered(
            'NotExistingCoveredElementTest', 'testOne'
        );
    }

    /**
     * @covers            UtilTest::getLinesToBeCovered
     * @covers            UtilTest::getLinesToBeCoveredOrUsed
     * @covers            UtilTest::resolveElementToReflectionObjects
     * @expectedException CodeCoverageException
     */
    public function testGetLinesToBeCovered3()
    {
        UtilTest::getLinesToBeCovered(
            'NotExistingCoveredElementTest', 'testTwo'
        );
    }

    /**
     * @covers            UtilTest::getLinesToBeCovered
     * @covers            UtilTest::getLinesToBeCoveredOrUsed
     * @covers            UtilTest::resolveElementToReflectionObjects
     * @expectedException CodeCoverageException
     */
    public function testGetLinesToBeCovered4()
    {
        UtilTest::getLinesToBeCovered(
            'NotExistingCoveredElementTest', 'testThree'
        );
    }

    /**
     * @covers UtilTest::getLinesToBeCovered
     * @covers UtilTest::getLinesToBeCoveredOrUsed
     */
    public function testGetLinesToBeCoveredSkipsNonExistentMethods()
    {
        $this->assertSame(
            [],
            UtilTest::getLinesToBeCovered(
                'NotExistingCoveredElementTest',
                'methodDoesNotExist'
            )
        );
    }

    /**
     * @covers            UtilTest::getLinesToBeCovered
     * @covers            UtilTest::getLinesToBeCoveredOrUsed
     * @expectedException CodeCoverageException
     */
    public function testTwoCoversDefaultClassAnnoationsAreNotAllowed()
    {
        UtilTest::getLinesToBeCovered(
            'CoverageTwoDefaultClassAnnotations',
            'testSomething'
        );
    }

    /**
     * @covers UtilTest::getLinesToBeCovered
     * @covers UtilTest::getLinesToBeCoveredOrUsed
     */
    public function testFunctionParenthesesAreAllowed()
    {
        $this->assertSame(
            [TEST_FILES_PATH . 'CoveredFunction.php' => range(2, 4)],
            UtilTest::getLinesToBeCovered(
                'CoverageFunctionParenthesesTest',
                'testSomething'
            )
        );
    }

    /**
     * @covers UtilTest::getLinesToBeCovered
     * @covers UtilTest::getLinesToBeCoveredOrUsed
     */
    public function testFunctionParenthesesAreAllowedWithWhitespace()
    {
        $this->assertSame(
            [TEST_FILES_PATH . 'CoveredFunction.php' => range(2, 4)],
            UtilTest::getLinesToBeCovered(
                'CoverageFunctionParenthesesWhitespaceTest',
                'testSomething'
            )
        );
    }

    /**
     * @covers UtilTest::getLinesToBeCovered
     * @covers UtilTest::getLinesToBeCoveredOrUsed
     */
    public function testMethodParenthesesAreAllowed()
    {
        $this->assertSame(
            [TEST_FILES_PATH . 'CoveredClass.php' => range(31, 35)],
            UtilTest::getLinesToBeCovered(
                'CoverageMethodParenthesesTest',
                'testSomething'
            )
        );
    }

    /**
     * @covers UtilTest::getLinesToBeCovered
     * @covers UtilTest::getLinesToBeCoveredOrUsed
     */
    public function testMethodParenthesesAreAllowedWithWhitespace()
    {
        $this->assertSame(
            [TEST_FILES_PATH . 'CoveredClass.php' => range(31, 35)],
            UtilTest::getLinesToBeCovered(
                'CoverageMethodParenthesesWhitespaceTest',
                'testSomething'
            )
        );
    }

    /**
     * @covers UtilTest::getLinesToBeCovered
     * @covers UtilTest::getLinesToBeCoveredOrUsed
     */
    public function testNamespacedFunctionCanBeCoveredOrUsed()
    {
        $this->assertEquals(
            [
                TEST_FILES_PATH . 'NamespaceCoveredFunction.php' => range(4, 7)
            ],
            UtilTest::getLinesToBeCovered(
                'CoverageNamespacedFunctionTest',
                'testFunc'
            )
        );
    }

    public function getLinesToBeCoveredProvider()
    {
        return [
          [
            'CoverageNoneTest',
            []
          ],
          [
            'CoverageClassExtendedTest',
            array_merge(range(19, 36), range(2, 17))
          ],
          [
            'CoverageClassTest',
            range(19, 36)
          ],
          [
            'CoverageMethodTest',
            range(31, 35)
          ],
          [
            'CoverageMethodOneLineAnnotationTest',
            range(31, 35)
          ],
          [
            'CoverageNotPrivateTest',
            array_merge(range(25, 29), range(31, 35))
          ],
          [
            'CoverageNotProtectedTest',
            array_merge(range(21, 23), range(31, 35))
          ],
          [
            'CoverageNotPublicTest',
            array_merge(range(21, 23), range(25, 29))
          ],
          [
            'CoveragePrivateTest',
            range(21, 23)
          ],
          [
            'CoverageProtectedTest',
            range(25, 29)
          ],
          [
            'CoveragePublicTest',
            range(31, 35)
          ],
          [
            'CoverageFunctionTest',
            range(2, 4)
          ],
          [
            'NamespaceCoverageClassExtendedTest',
            array_merge(range(21, 38), range(4, 19))
          ],
          [
            'NamespaceCoverageClassTest',
            range(21, 38)
          ],
          [
            'NamespaceCoverageMethodTest',
            range(33, 37)
          ],
          [
            'NamespaceCoverageNotPrivateTest',
            array_merge(range(27, 31), range(33, 37))
          ],
          [
            'NamespaceCoverageNotProtectedTest',
            array_merge(range(23, 25), range(33, 37))
          ],
          [
            'NamespaceCoverageNotPublicTest',
            array_merge(range(23, 25), range(27, 31))
          ],
          [
            'NamespaceCoveragePrivateTest',
            range(23, 25)
          ],
          [
            'NamespaceCoverageProtectedTest',
            range(27, 31)
          ],
          [
            'NamespaceCoveragePublicTest',
            range(33, 37)
          ],
          [
            'NamespaceCoverageCoversClassTest',
            array_merge(range(23, 25), range(27, 31), range(33, 37), range(6, 8), range(10, 13), range(15, 18))
          ],
          [
            'NamespaceCoverageCoversClassPublicTest',
            range(33, 37)
          ],
          [
            'CoverageNothingTest',
            false
          ]
        ];
    }
}
