<?php
require_once 'ChildSuite.php';

use Zynga\PHPUnit\V2\TestSuite;

class ParentSuite
{
    public static function suite()
    {
        $suite = new TestSuite('Parent');
        $suite->addTest(ChildSuite::suite());

        return $suite;
    }
}
