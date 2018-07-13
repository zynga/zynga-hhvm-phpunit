# zynga-hhvm-phpunit

Changes from vanilla phpunit here:
1. hhvm code coverage: The default coverage provided by the combo of phpunit and hhvm doesn't handle the hhvm changes well.
1. some path scanning functionality doesn't scale well with as large of a codebase we have, so some small tewaks on number of file scans and parses of code were done.
1. We utilize strict hhvm tests, made a change to TestSuite.php to support running non-static doBeforeClassSetup and doAfterClassSetup function instead of mixing modes between strict and non-strict code.

# TODO:

0) Before marking phpunit gtg for prod, need to verify that my simplified changes
   for the autoloader via composer.json, work correctly.

1) Need to find and eradicate all ReflectionClass and ReflectionMethod direct calls.
   Do we need to remove ReflectionObject?

2) Need to bring all the unit tests from phpunit and the child modules into a place 
   where they can be executed as part of this project.

3) Figure out a way to run the unit tests for phpunit as part of this project.
   Will require some changes as we've removed some functionality along the way.

# Need to port / make hh compliant:
* Symfony\Yaml\Dumper - Needs moving.
* CodeCoverage - Needs moving.

# Performance Benchmarks

Date:            | Time in Tests | Execution Time | Notes
--------------------------------------------------------------------------------
2018/07/05 10:00 | 34.81         | 53.72          | Timing method changed, run it once to allow for hhvm to compile, then run it again.
2018/07/05 23:47 | 34.12         | 52.31          | 

## Generating timings

make test # discard results
make test # record result


# History

The core developer has opened tickets with the hhvm team wanting for fixes to behave like php. It's unlikely the hhvm team will 
change to support the same functionality as they have added a improvement to xdebug showing you the 
number of times that a pieces of code is executed.

We have chosen to fork the phpunit library completly now, to make the whole stack
and its deps hacklang compliant.

# Licenses

We have preserved the top level directories of all the dependencies within
the third-party directory.

