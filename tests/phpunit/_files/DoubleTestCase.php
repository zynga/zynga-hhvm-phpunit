<?php

use Zynga\PHPUnit\V2\Interfaces\TestInterface;

class DoubleTestCase implements TestInterface
{
    protected $testCase;

    public function __construct(PHPUnit_Framework_TestCase $testCase)
    {
        $this->testCase = $testCase;
    }

    public function count()
    {
        return 2;
    }

    public function run(PHPUnit_Framework_TestResult $result = null)
    {
        $result->startTest($this);

        $this->testCase->runBare();
        $this->testCase->runBare();

        $result->endTest($this, 0);
    }
}
