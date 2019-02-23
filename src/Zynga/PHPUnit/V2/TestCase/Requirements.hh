<?hh // strict

namespace Zynga\PHPUnit\V2\TestCase;

use Zynga\PHPUnit\V2\TestCase;
use Zynga\PHPUnit\V2\Version;

class Requirements {
  // const string REGEX_PHPUNIT_VERSION = '/^PHPUnit\s*(.*)$/';
  const string
    REGEX_PHPUNIT_VERSION = '/(?P<name>PHPUnit?)\s+(?P<operator>[<>=!]{0,2})\s*(?P<version>[\d\.-]+(dev|(RC|alpha|beta)[\d\.])?)[ \t]*\r?$/m';
  const string
    REGEX_PHP_VERSION = '/(?P<name>PHP(?:Unit)?)\s+(?P<operator>[<>=!]{0,2})\s*(?P<version>[\d\.-]+(dev|(RC|alpha|beta)[\d\.])?)[ \t]*\r?$/m';
  const string REGEX_REQUIRES_OS = '/OS\s+(?P<value>.+?)[ \t]*\r?$/m';
  const string
    REGEX_REQUIRES_FUNCTION = '/(?P<name>function)\s+(?P<value>([^ ]+?))\s*(?P<operator>[<>=!]{0,2})\s*(?P<version>[\d\.-]+[\d\.]?)?[ \t]*\r?$/m';
  const string
    REGEX_REQUIRES_EXTENSION = '/(?P<name>extension)\s+(?P<value>([^ ]+?))\s*(?P<operator>[<>=!]{0,2})\s*(?P<version>[\d\.-]+[\d\.]?)?[ \t]*\r?$/m';

  public static function getMissingRequirements(
    TestCase $test,
  ): Vector<string> {

    $requires = $test->getAllAnnotationsForKey('requires');

    $missing = Vector {};

    if ($requires->count() == 0) {
      return $missing;
    }

    foreach ($requires as $require) {

      $requires_phpunit = self::handleRequire_PHPUnit_Version($require);

      if (is_string($requires_phpunit)) {
        $missing->add($requires_phpunit);
        continue;
      }

      $requires_php = self::handleRequire_PHP_Version($require);

      if (is_string($requires_php)) {
        $missing->add($requires_php);
        continue;
      }

      $requires_os = self::handleRequire_OS_Version($require);

      if (is_string($requires_os)) {
        $missing->add($requires_os);
        continue;
      }

      $requires_function = self::handleRequire_Function($require);

      if (is_string($requires_function)) {
        $missing->add($requires_function);
        continue;
      }

      $requires_extension = self::handleRequire_Extension($require);

      if (is_string($requires_extension)) {
        $missing->add($requires_extension);
        continue;
      }

    }

    // if ($requires->count() > 0) {
    //
    //   var_dump($requires);
    //   var_dump($missing);
    //   exit();
    //
    // }

    return $missing;

  }

  private static function handleRequire_Extension(
    string $annotation,
  ): ?string {

    $pregs = array();

    if (preg_match(self::REGEX_REQUIRES_EXTENSION, $annotation, $pregs)) {

      $extension = '';

      if (array_key_exists('value', $pregs)) {
        $extension = $pregs['value'];
      }

      $operator = '';
      if (array_key_exists('operator', $pregs)) {
        $operator = self::convertOperators($pregs['operator'], '>=');
      }

      $version = '';
      if (array_key_exists('version', $pregs)) {
        $version = $pregs['version'];
      }

      $extensionFound = false;

      if (extension_loaded($extension)) {
        $extensionFound = true;
      }

      if ($version != '') {

        if ($extensionFound == false) {
          return sprintf(
            'Extension %s is required to be %s %s.',
            $extension,
            $operator,
            $version,
          );
        }

        $actualVersion = phpversion($extension);

        $versionCompareResult =
          version_compare($actualVersion, $version, $operator);

        if ($versionCompareResult === false) {
          return sprintf(
            'Extension %s is required to be %s %s found %s.',
            $extension,
            $operator,
            $version,
            $actualVersion,
          );
        }

        return null;
      }

      if ($extensionFound != true) {
        return sprintf('Extension %s is required.', $extension);
      }

    }

    return null;

  }

  private static function handleRequire_Function(string $annotation): ?string {

    $pregs = array();

    if (preg_match(self::REGEX_REQUIRES_FUNCTION, $annotation, $pregs)) {
      $function = $pregs['value'];

      $pieces = explode('::', $function);
      if (2 === count($pieces) && method_exists($pieces[0], $pieces[1])) {
        // method exists.
        return null;
      }
      if (function_exists($function)) {
        // function exists
        return null;
      }

      return sprintf('Function %s is required.', $function);

    }

    return null;

  }

  private static function handleRequire_OS_Version(
    string $annotation,
  ): ?string {

    $pregs = array();

    if (preg_match(self::REGEX_REQUIRES_OS, $annotation, $pregs)) {

      $actualOS = PHP_OS;
      $expectedOS = $pregs['value'];
      $osRegExp = sprintf('/%s/i', addcslashes($expectedOS, '/'));

      if (!preg_match($osRegExp, $actualOS)) {
        return sprintf(
          'Operating system %s expected %s is required.',
          $actualOS,
          $osRegExp,
        );
      }

      return null;

    }

    return null;

  }

  private static function handleRequire_PHPUnit_Version(
    string $annotation,
  ): ?string {

    $hasVersion = self::handleRequire_Version(
      Version::VERSION_NUMBER,
      self::REGEX_PHPUNIT_VERSION,
      $annotation,
      'PHPUnit',
    );

    if (is_string($hasVersion) && $hasVersion != '') {
      return $hasVersion;
    }

    return null;

  }

  private static function handleRequire_PHP_Version(
    string $annotation,
  ): ?string {

    $hasVersion = self::handleRequire_Version(
      PHP_VERSION,
      self::REGEX_PHP_VERSION,
      $annotation,
      'PHP',
    );

    if (is_string($hasVersion) && $hasVersion != '') {
      return $hasVersion;
    }

    return null;

  }

  private static function handleRequire_Version(
    string $actualVersion,
    string $regex,
    string $annotation,
    string $versionPrefix,
  ): ?string {

    $pregs = array();
    if (preg_match($regex, $annotation, $pregs)) {

      $version = null;

      if (array_key_exists('version', $pregs)) {
        $version = $pregs['version'];
      }

      $operator = '';

      if (array_key_exists('operator', $pregs)) {
        $operator = $pregs['operator'];
      }

      $operator = self::convertOperators($operator, '>=');

      $versionCompareResult =
        version_compare($actualVersion, $version, $operator);

      if ($versionCompareResult === false) {
        return
          $versionPrefix.
          sprintf(
            ' version %s %s %s is required.',
            $actualVersion,
            $operator,
            $version,
          );
      }

      return null;

    }

    // There is no version requirement for this test/feature.
    return null;

  }

  private static function convertOperators(
    string $operator,
    string $default = '>=',
  ): string {

    // if it's a supported operator return it.
    if ($operator == '>=' ||
        $operator == '<=' ||
        $operator == '>' ||
        $operator == '<' ||
        $operator == '!=') {
      return $operator;
    }

    return $default;

  }

}
