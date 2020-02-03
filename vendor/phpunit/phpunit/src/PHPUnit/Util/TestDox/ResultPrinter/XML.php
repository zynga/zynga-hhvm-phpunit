<?php
/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use Zynga\Framework\ReflectionCache\V1\ReflectionClasses;
use Zynga\PHPUnit\V2\Interfaces\TestListenerInterface;
use Zynga\PHPUnit\V2\TestCase;
use Zynga\PHPUnit\V2\TestSuite;
/**
 * @since Class available since Release 5.4.0
 */
class PHPUnit_Util_TestDox_ResultPrinter_XML extends PHPUnit_Util_Printer implements TestListenerInterface
{
    /**
     * @var DOMDocument
     */
    private $document;

    /**
     * @var DOMElement
     */
    private $root;

    /**
     * @var PHPUnit_Util_TestDox_NamePrettifier
     */
    private $prettifier;

    /**
     * @var Exception
     */
    private $exception;

    /**
     * @param string|resource $out
     */
    public function __construct($out = null)
    {
        $this->document               = new DOMDocument('1.0', 'UTF-8');
        $this->document->formatOutput = true;

        $this->root = $this->document->createElement('tests');
        $this->document->appendChild($this->root);

        $this->prettifier = new PHPUnit_Util_TestDox_NamePrettifier;

        parent::__construct($out);
    }

    /**
     * Flush buffer and close output.
     */
    public function flush()
    {
        $this->write($this->document->saveXML());

        parent::flush();
    }

    /**
     * An error occurred.
     *
     * @param TestInterface $test
     * @param Exception              $e
     * @param float                  $time
     */
    public function addError(TestInterface $test, Exception $e, $time)
    {
        $this->exception = $e;
    }

    /**
     * A warning occurred.
     *
     * @param TestInterface    $test
     * @param Exception $e
     * @param float                     $time
     */
    public function addWarning(TestInterface $test, Exception $e, $time)
    {
    }

    /**
     * A failure occurred.
     *
     * @param TestInterface                 $test
     * @param Exception $e
     * @param float                                  $time
     */
    public function addFailure(TestInterface $test, Exception $e, $time)
    {
        $this->exception = $e;
    }

    /**
     * Incomplete test.
     *
     * @param TestInterface $test
     * @param Exception              $e
     * @param float                  $time
     */
    public function addIncompleteTest(TestInterface $test, Exception $e, $time)
    {
    }

    /**
     * Risky test.
     *
     * @param TestInterface $test
     * @param Exception              $e
     * @param float                  $time
     */
    public function addRiskyTest(TestInterface $test, Exception $e, $time)
    {
    }

    /**
     * Skipped test.
     *
     * @param TestInterface $test
     * @param Exception              $e
     * @param float                  $time
     */
    public function addSkippedTest(TestInterface $test, Exception $e, $time)
    {
    }

    /**
     * A test suite started.
     *
     * @param TestSuite $suite
     */
    public function startTestSuite(TestSuite $suite)
    {
    }

    /**
     * A test suite ended.
     *
     * @param TestSuite $suite
     */
    public function endTestSuite(TestSuite $suite)
    {
    }

    /**
     * A test started.
     *
     * @param TestInterface $test
     */
    public function startTest(TestInterface $test)
    {
        $this->exception = null;
    }

    /**
     * A test ended.
     *
     * @param TestInterface $test
     * @param float                  $time
     */
    public function endTest(TestInterface $test, $time)
    {
        if (!$test instanceof TestCase) {
            return;
        }

        $groups = array_filter(
            $test->getGroups(),
            function ($group) {
                if ($group == 'small' || $group == 'medium' || $group == 'large') {
                    return false;
                }

                return true;
            }
        );

        $node = $this->document->createElement('test');

        $node->setAttribute('className', get_class($test));
        $node->setAttribute('methodName', $test->getName());
        $node->setAttribute('prettifiedClassName', $this->prettifier->prettifyTestClass(get_class($test)));
        $node->setAttribute('prettifiedMethodName', $this->prettifier->prettifyTestMethod($test->getName()));
        $node->setAttribute('status', $test->getStatus());
        $node->setAttribute('time', $time);
        $node->setAttribute('size', $test->getSize());
        $node->setAttribute('groups', implode(',', $groups));

        $inlineAnnotations = PHPUnit_Util_Test::getInlineAnnotations(get_class($test), $test->getName());

        if (isset($inlineAnnotations['given']) && isset($inlineAnnotations['when']) && isset($inlineAnnotations['then'])) {
            $node->setAttribute('given', $inlineAnnotations['given']['value']);
            $node->setAttribute('givenStartLine', $inlineAnnotations['given']['line']);
            $node->setAttribute('when', $inlineAnnotations['when']['value']);
            $node->setAttribute('whenStartLine', $inlineAnnotations['when']['line']);
            $node->setAttribute('then', $inlineAnnotations['then']['value']);
            $node->setAttribute('thenStartLine', $inlineAnnotations['then']['line']);
        }

        if ($this->exception !== null) {
            if ($this->exception instanceof PHPUnit_Framework_Exception) {
                $steps = $this->exception->getSerializableTrace();
            } else {
                $steps = $this->exception->getTrace();
            }

            $class = ReflectionClasses::getReflection($test);
            $file  = $class->getFileName();

            foreach ($steps as $step) {
                if (isset($step['file']) && $step['file'] == $file) {
                    $node->setAttribute('exceptionLine', $step['line']);

                    break;
                }
            }

            $node->setAttribute('exceptionMessage', $this->exception->getMessage());
        }

        $this->root->appendChild($node);
    }
}
