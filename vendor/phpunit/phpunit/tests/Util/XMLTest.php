<?php
/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use PHPUnit\Exceptions\Exception as PHPUnit_Exceptions_Exception;
use PHPUnit\Util\UtilXML;

/**
 * @since      Class available since Release 3.3.0
 * @covers     UtilXML
 */
class Util_XMLTest extends PHPUnit_Framework_TestCase
{
    /**
     * @dataProvider charProvider
     */
    public function testPrepareString($char)
    {
        $e = null;

        $escapedString = UtilXML::prepareString($char);
        $xml           = "<?xml version='1.0' encoding='UTF-8' ?><tag>$escapedString</tag>";
        $dom           = new DomDocument('1.0', 'UTF-8');

        try {
            $dom->loadXML($xml);
        } catch (Exception $e) {
        }

        $this->assertNull($e, sprintf(
            'UtilXML::prepareString("\x%02x") should not crash DomDocument',
            ord($char)
        ));
    }

    public function charProvider()
    {
        $data = [];

        for ($i = 0; $i < 256; $i++) {
            $data[] = [chr($i)];
        }

        return $data;
    }

    /**
     * @expectedException PHPUnit_Exceptions_Exception
     * @expectedExceptionMessage Could not load XML from empty string
     */
    public function testLoadEmptyString()
    {
        UtilXML::load('');
    }

    /**
     * @expectedException PHPUnit_Exceptions_Exception
     * @expectedExceptionMessage Could not load XML from array
     */
    public function testLoadArray()
    {
        UtilXML::load([1, 2, 3]);
    }

    /**
     * @expectedException PHPUnit_Exceptions_Exception
     * @expectedExceptionMessage Could not load XML from boolean
     */
    public function testLoadBoolean()
    {
        UtilXML::load(false);
    }
}
