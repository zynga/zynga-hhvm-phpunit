<?php

use Zynga\PHPUnit\V2\TestSuite;

class StopOnWarningTestSuite
{
    public static function suite()
    {
        $suite = new TestSuite('Test Warnings');

        $suite->addTestSuite('NoTestCases');
        $suite->addTestSuite('CoverageClassTest');

        return $suite;
    }
}
