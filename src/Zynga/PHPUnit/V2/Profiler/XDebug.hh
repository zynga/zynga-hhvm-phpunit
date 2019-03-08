<?hh

namespace Zynga\PHPUnit\V2\Profiler;

class XDebug {

  public static function getMonitoredFunctions(): Vector<Map<string, mixed>> {
    $functions = xdebug_get_monitored_functions();

    $data = Vector {};

    if (is_array($functions)) {
      foreach ($functions as $function) {
        $functionMap = Map {};
        if (array_key_exists('function', $function)) {
          $functionMap['function'] = $function['function'];
        }
        if (array_key_exists('filename', $function)) {
          $functionMap['filename'] = $function['filename'];
        }
        if (array_key_exists('lineno', $function)) {
          $functionMap['lineno'] = $function['lineno'];
        }
        $data->add($functionMap);
      }
    }

    return $data;

  }

  public static function startMonitoringFunctions(
    Vector<string> $functions,
  ): void {
    xdebug_start_function_monitor($functions->toArray());
  }

  public static function stopMonitoringFunctions(): void {
    xdebug_stop_function_monitor();
  }

}
