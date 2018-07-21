# zynga-hhvm-phpunit

Changes from vanilla phpunit here:
1. hhvm code coverage: The default coverage provided by the combo of phpunit and hhvm doesn't handle the hhvm changes well.
1. some path scanning functionality doesn't scale well with as large of a codebase we have, so some small tewaks on number of file scans and parses of code were done.
1. We utilize strict hhvm tests, made a change to TestSuite.php to support running non-static doBeforeClassSetup and doAfterClassSetup function instead of mixing modes between strict and non-strict code.

# TODO
- phpunit-vendor-xdebug deps against zynga specific stuff, need to clean that up into usable.

# History

The core developer has opened tickets with the hhvm team wanting for fixes to behave like php. It's unlikely the hhvm team will 
change to support the same functionality as they have added a improvement to xdebug showing you the 
number of times that a pieces of code is executed.
