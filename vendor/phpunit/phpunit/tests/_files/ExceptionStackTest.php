<?php

use PHPUnit\Exceptions\Exception as PHPUnit_Exceptions_Exception;

class ExceptionStackTest extends PHPUnit_Framework_TestCase
{
    public function testPrintingChildException()
    {
        try {
            $this->assertEquals([1], [2], 'message');
        } catch (PHPUnit_Framework_ExpectationFailedException $e) {
            $message = $e->getMessage() . $e->getComparisonFailure()->getDiff();
            throw new PHPUnit_Exceptions_Exception("Child exception\n$message", 101, $e);
        }
    }

    public function testNestedExceptions()
    {
        $exceptionThree = new Exception('Three');
        $exceptionTwo   = new InvalidArgumentException('Two', 0, $exceptionThree);
        $exceptionOne   = new Exception('One', 0, $exceptionTwo);
        throw $exceptionOne;
    }
}
