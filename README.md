# zynga-hhvm-phpunit

Couple major changes from vanilla phpunit here:
- hhvm code coverage: The default coverage provided by the combo of phpunit and hhvm doesn't handle the hhvm changes well. The core developer has opened tickets with the hhvm team wanting for fixes to behave like php. It's unlikely the hhvm team will change to support the same functionality as they have added a improvement to xdebug showing you the number of times that a pieces of code is executed.
- some path scanning functionality doesn't scale well with as large of a codebase we have, so some small tewaks on number of file scans and parses of code were done.
