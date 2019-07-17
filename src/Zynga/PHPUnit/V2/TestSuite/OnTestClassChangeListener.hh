<?hh // strict

namespace Zynga\PHPUnit\V2\TestSuite;

use Zynga\PHPUnit\V2\Interfaces\TestInterface;
use Zynga\Framework\Dynamic\V1\DynamicMethodCall;
use \Exception;

class OnTestClassChangeListener {
  private static ?TestInterface $_currentClass = null;

  public static function isClassChange(TestInterface $candidate): bool {
    $candidateName = get_class($candidate);

    if (self::$_currentClass instanceof TestInterface) {
      $currentName = get_class(self::$_currentClass);
      if ($candidateName != $currentName) {
        return true;
      }
      return false;
    }

    // nothing set for the currentClass
    return true;

  }

  public static function setClass(TestInterface $class): bool {
    self::$_currentClass = $class;
    return true;
  }

  public static function handleBeforeClass(
    TestInterface $test,
    Vector<string> $beforeClassMethods,
  ): (bool, ?Exception) {

    try {

      foreach ($beforeClassMethods as $beforeClassMethod) {

        DynamicMethodCall::callMethodOnObject(
          $test,
          $beforeClassMethod,
          Vector {},
          false,
          true,
        );
      }

    } catch (Exception $e) {
      return tuple(false, $e);
    }

    return tuple(true, null);

  }

  public static function handleAfterClass(
    Vector<string> $afterClassMethods,
  ): (bool, ?Exception) {

    $test = self::$_currentClass;

    // no after class to call.
    if (!self::$_currentClass instanceof TestInterface) {
      return tuple(true, null);
    }

    try {

      foreach ($afterClassMethods as $afterClassMethod) {
        DynamicMethodCall::callMethodOnObject(
          $test,
          $afterClassMethod,
          Vector {},
          false,
          true,
        );
      }

    } catch (Exception $e) {
      return tuple(false, $e);
    }

    return tuple(true, null);

  }

}
