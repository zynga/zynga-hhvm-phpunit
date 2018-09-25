<?php
/**
 * Some comment
 */
class Foo{function foo(){}

    /**
     * @param Baz $baz
     */
    public function bar(Baz $baz)
    {
    }

    /**
     * @param Foobar $foobar
     */
    static public function foobar(Foobar $foobar)
    {
    }

    public function barfoo(Barfoo $barfoo)
    {
    }

    /**
     * This docblock does not belong to the baz function, except in the zynga version where we are more permissive in whitespace parsing.
     */

    public function baz()
    {
    }

    public function blaz($x, $y)
    {
    }
}
