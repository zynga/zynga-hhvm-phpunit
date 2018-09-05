# zynga-hhvm-phpunit

Changes from vanilla phpunit here:
1. hhvm code coverage: The default coverage provided by the combo of phpunit and hhvm doesn't handle the hhvm changes well.
1. some path scanning functionality doesn't scale well with as large of a codebase we have, so some small tweaks on number of file scans and parses of code were done.
1. We utilize strict hhvm tests, made a change to TestSuite.php to support running non-static doBeforeClassSetup and doAfterClassSetup function instead of mixing modes between strict and non-strict code.
1. Fixed: Multiline use statements showing up as uncovered.
1. New: Added support for enums spanning multiple lines.
1. Converted all of php-token-stream to be hack strict.
1. Moved 'most' of php-code-coverage into hack, some strict, some not.
1. Rewrote a significant portion of how the code coverage stack is handled in order to lower memory footprint and not pass around the line status stack constantly.
1. Made the code view use solarized dark as the default for viewing code.
1. Extracted more of the html from the render to the template structure.
1. Collapsed all of the HHVM driver code to be within the "HHVM" driver, in preparation for supporting the newer debuggers available.
1. Changed ordering of classes, functions, lines to be consistent throughout the UI.
1. Removed / reduced the usage of TextTemplate usage within the html template, as we have a very large code base the amount of str_replace overhead catches up with you.
1. Removed the file require|inclusion within code coverage. This lead to older php code being blind executed. Also hid some of the performance issues around loading of certain legacy libraries. 

# TODO  
1. SebastianBergmann\TokenStream\Token\StreamMethodStructure->methodSignature isn't working correctly, needs fixing. Might be only multi-line function signatures that are broken - TBD.
1. PHP_Token_Function: Needs a verification that it handles marking a multi-line abstract function definition as non-executable.
1. LineToTokens should be moved into stream functionality.
1. Need to repair the dashboard functionality, as it's fully broken right now.
1. Performance profiling for the report generation needs to continue.

# History

The core developer has opened tickets with the HHVM team wanting for fixes to behave like php. It's unlikely the HHVM team will
change to support the same functionality as they have added a improvement to XDebug showing you the
number of times that a pieces of code is executed.
