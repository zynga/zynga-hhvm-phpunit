<?php
require_once 'OneTest.php';
require_once 'TwoTest.php';

use Zynga\PHPUnit\V2\TestSuite;

class ChildSuite
{
    public static function suite()
    {
        $suite = new TestSuite('Child');
        $suite->addTestSuite('OneTest');
        $suite->addTestSuite('TwoTest');

        return $suite;
    }
}
