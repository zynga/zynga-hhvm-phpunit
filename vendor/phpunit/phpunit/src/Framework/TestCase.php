<?php

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use SebastianBergmann\Exporter\Exporter;
use SebastianBergmann\PHPUnit\Environment\OutputBuffering;
use SebastianBergmann\PHPUnit\Exceptions\AssertionFailedException;
use SebastianBergmann\PHPUnit\Exceptions\WarningException;
use SebastianBergmann\PHPUnit\Exceptions\TestError\IncompleteException;
use SebastianBergmann\PHPUnit\Exceptions\TestError\RiskyException;
use SebastianBergmann\PHPUnit\Exceptions\TestError\SkippedException;
use DeepCopy\DeepCopy;
use Zynga\Framework\ReflectionCache\V1\ReflectionClasses;
use Zynga\PHPUnit\V2\TestCase;

use \Throwable;
use \Exception;

/**
 * A TestCase defines the fixture to run multiple tests.
 *
 * To define a TestCase
 *
 *   1) Implement a subclass of PHPUnit_Framework_TestCase.
 *   2) Define instance variables that store the state of the fixture.
 *   3) Initialize the fixture state by overriding setUp().
 *   4) Clean-up after a test by overriding tearDown().
 *
 * Each test runs in its own fixture so there can be no side effects
 * among test runs.
 *
 * Here is an example:
 *
 * <code>
 * <?php
 * class MathTest extends PHPUnit_Framework_TestCase
 * {
 *     public $value1;
 *     public $value2;
 *
 *     protected function setUp()
 *     {
 *         $this->value1 = 2;
 *         $this->value2 = 3;
 *     }
 * }
 * ?>
 * </code>
 *
 * For each test implement a method which interacts with the fixture.
 * Verify the expected results with assertions specified by calling
 * assert with a boolean.
 *
 * <code>
 * <?php
 * public function testPass()
 * {
 *     $this->assertTrue($this->value1 + $this->value2 == 5);
 * }
 * ?>
 * </code>
 *
 * @since Class available since Release 2.0.0
 */
// abstract class PHPUnit_Framework_TestCase extends TestCase implements TestInterface, PHPUnit_Framework_SelfDescribing
// {
//
//     /**
//      * Constructs a test case with the given name.
//      *
//      * @param string $name
//      * @param array  $data
//      * @param string $dataName
//      */
//     public function __construct($name = null, array $data = [], $dataName = '')
//     {
//
//       // call the hack strict testcase constructor.
//       parent::__construct(strval($name), $data, strval($dataName));
//
//     }
//
//     public function getTest(): TestInterface {
//       return $this;
//     }
//
// }
