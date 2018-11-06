<?hh // strict

namespace SebastianBergmann\PHPUnit\Exceptions;

// --
// We removed the process isolation component from phpunit so we were able to
// simplify the base exception pretty significantly.
// --

use SebastianBergmann\PHPUnit\Interfaces\ExceptionInterface;
use \RuntimeException;

class Exception extends RuntimeException implements ExceptionInterface {}
