<?hh // strict

namespace Zynga\PHPUnit\V2\TestSuite;

use Zynga\PHPUnit\V2\Interfaces\TestInterface;
use Zynga\Framework\Dynamic\V1\DynamicMethodCall;
use \Exception;

class OnTestClassChangeListener {
  private static ?TestInterface $_currentClass = null;

  public static function isClassChange(TestInterface $candidate): bool {

    if (self::$_currentClass == null) {
      return true;
    }

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
  ): (bool, ?Exception) {

    try {

      $hooks = $test->getHookMethods();

      $beforeClassMethods = $hooks->get('beforeClass');

      if ($beforeClassMethods instanceof Vector) {

        foreach ($beforeClassMethods as $beforeClassMethod) {

          // var_dump(
          //   'CALLING_BEFORE class='.get_class($test).'::'.$beforeClassMethod,
          // );

          DynamicMethodCall::callMethodOnObject(
            $test,
            $beforeClassMethod,
            Vector {},
            false,
            true,
          );

        }

      }

    } catch (Exception $e) {
      return tuple(false, $e);
    }

    return tuple(true, null);

  }

  public static function handleAfterClass(): (bool, ?Exception) {

    $test = self::$_currentClass;

    // no after class to call.
    if (!$test instanceof TestInterface) {
      return tuple(true, null);
    }

    try {

      $hooks = $test->getHookMethods();

      $afterClassMethods = $hooks->get('afterClass');

      if ($afterClassMethods instanceof Vector) {
        foreach ($afterClassMethods as $afterClassMethod) {

          // var_dump(
          //   'CALLING_AFTER class='.get_class($test).'::'.$afterClassMethod,
          // );

          DynamicMethodCall::callMethodOnObject(
            $test,
            $afterClassMethod,
            Vector {},
            false,
            true,
          );

        }
      }

    } catch (Exception $e) {
      return tuple(false, $e);
    }

    // deallocate our pointer to the currentClass as we've called a after moment.
    self::$_currentClass = null;

    return tuple(true, null);

  }

}
