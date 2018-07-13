<?hh // strict

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace PHPUnit\Util;

use Zynga\Framework\Annotations\V1\ClassAnnotations;
use Zynga\Framework\Annotations\V1\MethodAnnotations;

use Zynga\Framework\ReflectionCache\V1\Reflectable;
use Zynga\Framework\ReflectionCache\V1\ReflectionClasses;
use Zynga\Framework\ReflectionCache\V1\ReflectionFunctions;
use Zynga\Framework\ReflectionCache\V1\ReflectionMethods;
use Zynga\Framework\ReflectionCache\V1\ReflectionClassDocComments;
use Zynga\Framework\ReflectionCache\V1\ReflectionMethodDocComments;

use PHPUnit\Exceptions\CodeCoverageException;
use PHPUnit\Exceptions\InvalidCoversTargetException;
use PHPUnit\Exceptions\Exception as PHPUnit_Exceptions_Exception;
use PHPUnit\Interfaces\TestInterface;
use PHPUnit\Interfaces\SelfDescribingInterface;
use PHPUnit\Util\TestUtil\Annotation;
use PHPUnit\Util\TestUtil\MethodAnnotation;

use \ReflectionMethod;
use \ReflectionFunction;
use \ReflectionException;
use \ReflectionClass;

/**
 * Test helpers.
 *
 * @since Class available since Release 3.0.0
 */
class TestUtil {
    const REGEX_DATA_PROVIDER      = '/@dataProvider\s+([a-zA-Z0-9._:-\\\\x7f-\xff]+)/';
    const REGEX_TEST_WITH          = '/@testWith\s+/';
    const REGEX_EXPECTED_EXCEPTION = '(@expectedException\s+([:.\w\\\\x7f-\xff]+)(?:[\t ]+(\S*))?(?:[\t ]+(\S*))?\s*$)m';
    const REGEX_REQUIRES_VERSION   = '/@requires\s+(?P<name>PHP(?:Unit)?)\s+(?P<operator>[<>=!]{0,2})\s*(?P<version>[\d\.-]+(dev|(RC|alpha|beta)[\d\.])?)[ \t]*\r?$/m';
    const REGEX_REQUIRES_OS        = '/@requires\s+OS\s+(?P<value>.+?)[ \t]*\r?$/m';
    const REGEX_REQUIRES           = '/@requires\s+(?P<name>function|extension)\s+(?P<value>([^ ]+?))\s*(?P<operator>[<>=!]{0,2})\s*(?P<version>[\d\.-]+[\d\.]?)?[ \t]*\r?$/m';

    const UNKNOWN = -1;
    const SMALL   = 0;
    const MEDIUM  = 1;
    const LARGE   = 2;

    //JEO: Disabled the hook methods variable, as we disabled the dynamic
    // function calling in zynga fork.
    //private static $hookMethods = [];

    /**
     * @param TestInterface $test
     * @param bool                   $asString
     *
     * @return mixed
     */
    public static function describe(TestInterface $test, bool $asString = true): mixed {
        if ($asString) {
            if ($test instanceof SelfDescribingInterface) {
                return $test->toString();
            } else {
                return get_class($test);
            }
        } else {
            if ($test instanceof TestInterface) {
                return [
                  get_class($test), $test->getName()
                ];
            } elseif ($test instanceof SelfDescribingInterface) {
                return ['', $test->toString()];
            } else {
                return ['', get_class($test)];
            }
        }
    }

    /**
     * @param string $className
     * @param string $methodName
     *
     * @return array|bool
     *
     * @throws CodeCoverageException
     *
     * @since Method available since Release 4.0.0
     */
    public static function getLinesToBeCovered(string $className, string $methodName): Map<string, Vector<int>> {

        $annotations = self::parseTestMethodAnnotations(
            $className,
            $methodName
        );

        if (
          $annotations->classAnnotations->containsKey('coversNothing') ||
          $annotations->methodAnnotations->containsKey('coversNothing') ) {
            return Map {};
          }

        return self::getLinesToBeCoveredOrUsed($className, $methodName, 'covers');

    }

    /**
     * Returns lines of code specified with the @uses annotation.
     *
     * @param string $className
     * @param string $methodName
     *
     * @return array
     *
     * @since Method available since Release 4.0.0
     */
    public static function getLinesToBeUsed(string $className, string $methodName): Map<string, Vector<int>>
    {
        return self::getLinesToBeCoveredOrUsed($className, $methodName, 'uses');
    }

    /**
     * @param string $className
     * @param string $methodName
     * @param string $mode
     *
     * @return array
     *
     * @throws CodeCoverageException
     *
     * @since Method available since Release 4.2.0
     */
    private static function getLinesToBeCoveredOrUsed(string $className, string $methodName, string $mode): Map<string, Vector<int>> {

      $annotations = self::parseTestMethodAnnotations(
            $className,
            $methodName
        );

      $defaultClassKey = $mode . 'DefaultClass';

      $classShortcut = null;

      if ( $annotations->classAnnotations->containsKey($defaultClassKey) ) {
        if (
          is_array($annotations->classAnnotations->containsKey($defaultClassKey)) &&
          count($annotations->classAnnotations->get($defaultClassKey)) > 1
          ) {
            throw new CodeCoverageException(
              sprintf(
                'More than one @%sClass annotation in class or interface "%s".',
                $mode,
                $className
              )
            );
          }

        $classShortcut = $annotations->classAnnotations->get($defaultClassKey);

      }

      $list = Vector {};

      if ( $annotations->classAnnotations->containsKey($mode) === true ) {
        $data = $annotations->classAnnotations->get($mode);
        if ( is_array($data) ) {
          foreach ( $data as $elem ) {
            $list->add($elem);
          }
        }
      }

      if ( $annotations->methodAnnotations->containsKey($mode) === true ) {
        $data = $annotations->methodAnnotations->get($mode);
        if ( is_array($data) ) {
          foreach ( $data as $elem ) {
            $list->add($elem);
          }
        }
      }

      $codeList = Vector {};

      foreach (array_unique($list->toArray()) as $element) {

        if ($classShortcut && strncmp($element, '::', 2) === 0) {
          $element = $classShortcut . $element;
        }

        $element = preg_replace('/[\s()]+$/', '', $element);
        $element = explode(' ', $element);
        $element = $element[0];

        $reflectionObject = self::resolveElementToReflectionObjects($element);

        $codeList->add($reflectionObject);
      }

      return self::resolveReflectionObjectsToLines($codeList);

    }

    /**
     * Returns the requirements for a test.
     *
     * @param string $className
     * @param string $methodName
     *
     * @return array
     *
     * @since Method available since Release 3.6.0
     */
    public static function getRequirements(string $className, string $methodName): Map<string, mixed> {
      $requires = Map {};
      return $requires;

      // JEO: Disabled for now.
      /*
      $classComment = ReflectionClassDocComments::getDocComment($className);
      $methodComment = ReflectionMethodDocComments::getDocComment($className, $methodName);

      $docComment = '';

      if ( $methodComment !== null ) {
        $docComment .= $methodComment;
      }

      if ( $classComment !== null ) {
        $docComment .= "\n";
        $docComment .= $classComment;
      }
      */


      // --
      // JEO: Unknown if we need to support this anymore.
      // --
      /*
        if ($count = preg_match_all(self::REGEX_REQUIRES_OS, $docComment, $matches)) {
            $requires['OS'] = sprintf(
                '/%s/i',
                addcslashes($matches['value'][$count - 1], '/')
            );
        }
        if ($count = preg_match_all(self::REGEX_REQUIRES_VERSION, $docComment, $matches)) {
            for ($i = 0; $i < $count; $i++) {
                $requires[$matches['name'][$i]] = [
                    'version'  => $matches['version'][$i],
                    'operator' => $matches['operator'][$i]
                ];
            }
        }

        // https://bugs.php.net/bug.php?id=63055
        $matches = array();

        if ($count = preg_match_all(self::REGEX_REQUIRES, $docComment, $matches)) {
            for ($i = 0; $i < $count; $i++) {
                $name = $matches['name'][$i] . 's';
                if (!isset($requires[$name])) {
                    $requires[$name] = [];
                }
                $requires[$name][] = $matches['value'][$i];
                if (empty($matches['version'][$i]) || $name != 'extensions') {
                    continue;
                }
                $requires['extension_versions'][$matches['value'][$i]] = [
                    'version'  => $matches['version'][$i],
                    'operator' => $matches['operator'][$i]
                ];
            }
        }
        */

    }

    /**
     * Returns the missing requirements for a test.
     *
     * @param string $className
     * @param string $methodName
     *
     * @return array
     *
     * @since Method available since Release 4.3.0
     */
    public static function getMissingRequirements(string $className, string $methodName): Vector<string> {
      // JEO: This is disabled just like getRequirements for now.
      return Vector {};
      /*
        $required = static::getRequirements($className, $methodName);
        $missing  = [];

        $operator = empty($required['PHP']['operator']) ? '>=' : $required['PHP']['operator'];
        if (!empty($required['PHP']) && !version_compare(PHP_VERSION, $required['PHP']['version'], $operator)) {
            $missing[] = sprintf('PHP %s %s is required.', $operator, $required['PHP']['version']);
        }

        if (!empty($required['PHPUnit'])) {
            $phpunitVersion = PHPUnit_Runner_Version::id();

            $operator = empty($required['PHPUnit']['operator']) ? '>=' : $required['PHPUnit']['operator'];
            if (!version_compare($phpunitVersion, $required['PHPUnit']['version'], $operator)) {
                $missing[] = sprintf('PHPUnit %s %s is required.', $operator, $required['PHPUnit']['version']);
            }
        }

        if (!empty($required['OS']) && !preg_match($required['OS'], PHP_OS)) {
            $missing[] = sprintf('Operating system matching %s is required.', $required['OS']);
        }

        if (!empty($required['functions'])) {
            foreach ($required['functions'] as $function) {
                $pieces = explode('::', $function);
                if (2 === count($pieces) && method_exists($pieces[0], $pieces[1])) {
                    continue;
                }
                if (function_exists($function)) {
                    continue;
                }
                $missing[] = sprintf('Function %s is required.', $function);
            }
        }

        if (!empty($required['extensions'])) {
            foreach ($required['extensions'] as $extension) {
                if (isset($required['extension_versions'][$extension])) {
                    continue;
                }
                if (!extension_loaded($extension)) {
                    $missing[] = sprintf('Extension %s is required.', $extension);
                }
            }
        }

        if (!empty($required['extension_versions'])) {
            foreach ($required['extension_versions'] as $extension => $required) {
                $actualVersion = phpversion($extension);

                $operator = empty($required['operator']) ? '>=' : $required['operator'];
                if (false === $actualVersion || !version_compare($actualVersion, $required['version'], $operator)) {
                    $missing[] = sprintf('Extension %s %s %s is required.', $extension, $operator, $required['version']);
                }
            }
        }

        return $missing;
        */
    }

    /**
     * Returns the expected exception for a test.
     *
     * @param string $className
     * @param string $methodName
     *
     * @return array
     *
     * @since Method available since Release 3.3.6
     */
    public static function getExpectedException(string $className, string $methodName): Map<string, mixed> {

      $docComment = ReflectionMethodDocComments::getDocComment($className, $methodName);

      $docComment = substr($docComment, 3, -2);

      $matches = array();
      if (preg_match(self::REGEX_EXPECTED_EXCEPTION, $docComment, $matches)) {

        $annotations = self::parseTestMethodAnnotations(
            $className,
            $methodName
        );

        $class         = $matches[1];
        $code          = null;
        $message       = '';
        $messageRegExp = '';

        if (array_key_exists(2, $matches)) {
          $message = trim($matches[2]);
        } elseif ($annotations->methodAnnotations->containsKey('expectedExceptionMessage') ) {
          $message = self::parseAnnotationContent(
             strval($annotations->methodAnnotations->get('expectedExceptionMessage'))
          );
        }

        if ($annotations->methodAnnotations->containsKey('expectedExceptionMessageRegExp')) {
          $messageRegExp = self::parseAnnotationContent(
            strval($annotations->methodAnnotations->get('expectedExceptionMessageRegExp'))
          );
        }

        if (array_key_exists(3, $matches)) {
          $code = $matches[3];
        } elseif ($annotations->methodAnnotations->containsKey('expectedExceptionCode')) {
          $code = self::parseAnnotationContent(
            strval($annotations->methodAnnotations->get('expectedExceptionCode'))
          );
        }

        if (is_numeric($code)) {
          $code = (int) $code;
        } elseif (is_string($code) && defined($code)) {
          $code = (int) constant($code);
        }

        return Map {
          'class' => $class, 'code' => $code, 'message' => $message, 'message_regex' => $messageRegExp
        };

      }

      return Map {};

    }

    /**
     * Parse annotation content to use constant/class constant values
     *
     * Constants are specified using a starting '@'. For example: @ClassName::CONST_NAME
     *
     * If the constant is not found the string is used as is to ensure maximum BC.
     *
     * @param string $message
     *
     * @return string
     */
    private static function parseAnnotationContent(string $message): string {

        if (strpos($message, '::') !== false && count(explode('::', $message) == 2)) {
            if (defined($message)) {
                $message = constant($message);
            }
        }

        return $message;
    }

    /**
     * Returns the provided data for a method.
     *
     * @param string $className
     * @param string $methodName
     *
     * @return array|Iterator when a data provider is specified and exists
     *                        null           when no data provider is specified
     *
     * @throws PHPUnit_Exceptions_Exception
     *
     * @since Method available since Release 3.2.0
     */
    public static function getProvidedData(string $className, string $methodName): void {
      return;
      /*
        $reflector  = new ReflectionMethod($className, $methodName);
        $docComment = $reflector->getDocComment();
        $data       = null;

        if ($dataProviderData = self::getDataFromDataProviderAnnotation($docComment, $className, $methodName)) {
            $data = $dataProviderData;
        }

        if ($testWithData = self::getDataFromTestWithAnnotation($docComment)) {
            $data = $testWithData;
        }

        if ($data !== null) {
            if (is_object($data)) {
                $data = iterator_to_array($data);
            }

            foreach ($data as $key => $value) {
                if (!is_array($value)) {
                    throw new PHPUnit_Exceptions_Exception(
                        sprintf(
                            'Data set %s is invalid.',
                            is_int($key) ? '#' . $key : '"' . $key . '"'
                        )
                    );
                }
            }
        }

        return $data;
        */
    }

    /**
     * Returns the provided data for a method.
     *
     * @param string $docComment
     * @param string $className
     * @param string $methodName
     *
     * @return array|Iterator when a data provider is specified and exists
     *                        null           when no data provider is specified
     *
     * @throws PHPUnit_Exceptions_Exception
     */
    private static function getDataFromDataProviderAnnotation(string $docComment, string $className, string $methodName): ?Vector<mixed> {
      return null;
      /*
        if (preg_match(self::REGEX_DATA_PROVIDER, $docComment, $matches)) {
            $dataProviderMethodNameNamespace = explode('\\', $matches[1]);
            $leaf                            = explode('::', array_pop($dataProviderMethodNameNamespace));
            $dataProviderMethodName          = array_pop($leaf);

            if (!empty($dataProviderMethodNameNamespace)) {
                $dataProviderMethodNameNamespace = implode('\\', $dataProviderMethodNameNamespace) . '\\';
            } else {
                $dataProviderMethodNameNamespace = '';
            }

            if (!empty($leaf)) {
                $dataProviderClassName = $dataProviderMethodNameNamespace . array_pop($leaf);
            } else {
                $dataProviderClassName = $className;
            }

            $dataProviderClass  = new ReflectionClass($dataProviderClassName);
            $dataProviderMethod = $dataProviderClass->getMethod(
                $dataProviderMethodName
            );

            if ($dataProviderMethod->isStatic()) {
                $object = null;
            } else {
                $object = $dataProviderClass->newInstance();
            }

            if ($dataProviderMethod->getNumberOfParameters() == 0) {
                $data = $dataProviderMethod->invoke($object);
            } else {
                $data = $dataProviderMethod->invoke($object, $methodName);
            }

            return $data;
        }
      */
    }

    /**
     * @param string $docComment full docComment string
     *
     * @return array when @testWith annotation is defined
     *               null  when @testWith annotation is omitted
     *
     * @throws PHPUnit_Exceptions_Exception when @testWith annotation is defined but cannot be parsed
     */
    public static function getDataFromTestWithAnnotation(string $docComment): Vector<mixed> {
      return Vector {};
      /*
        $docComment = self::cleanUpMultiLineAnnotation($docComment);

        $matches = array();
        if (preg_match(self::REGEX_TEST_WITH, $docComment, $matches, PREG_OFFSET_CAPTURE)) {
            $offset            = strlen($matches[0][0]) + $matches[0][1];
            $annotationContent = substr($docComment, $offset);
            $data              = [];
            foreach (explode("\n", $annotationContent) as $candidateRow) {
                $candidateRow = trim($candidateRow);
                $dataSet      = json_decode($candidateRow, true);
                if (json_last_error() != JSON_ERROR_NONE) {
                    break;
                }
                $data[] = $dataSet;
            }

            if (!$data) {
                throw new PHPUnit_Exceptions_Exception('The dataset for the @testWith annotation cannot be parsed.');
            }

            return $data;
        }
        */
    }

    private static function cleanUpMultiLineAnnotation(string $docComment): string {
        //removing initial '   * ' for docComment
        $docComment = preg_replace('/' . '\n' . '\s*' . '\*' . '\s?' . '/', "\n", $docComment);
        $docComment = substr($docComment, 0, -1);
        $docComment = rtrim($docComment, "\n");

        return $docComment;
    }

    /**
     * @param string $className
     * @param string $methodName
     *
     * @return array
     *
     * @throws ReflectionException
     *
     * @since Method available since Release 3.4.0
     */
    public static function parseTestMethodAnnotations(string $className, string $methodName = ''): MethodAnnotation {

      // We know for sure that we should be parsing for the class level annotations.
      $classAnnotations = ClassAnnotations::getAnnotations($className);
      $methodAnnotations = Vector {};

      if ( $methodName !== '' ) {
        $methodAnnotations = MethodAnnotations::getAnnotations($className, $methodName);
      }

      $data = new MethodAnnotation($classAnnotations, $methodAnnotations);

      return $data;

    }

    /**
     * @param string $className
     * @param string $methodName
     *
     * @return array
     *
     * @since Method available since Release 5.4.0
     */
    public static function getInlineAnnotations(string $className, string $methodName): Map<string, Map<string, string>>
    {
        $method      = ReflectionMethods::getReflection($className, $methodName);

        if ( ! $method instanceof ReflectionMethod ) {
          return Map {};
        }

        $code        = file($method->getFileName());
        $lineNumber  = intval($method->getStartLine());
        $startLine   = intval($method->getStartLine()) - 1;
        $endLine     = intval($method->getEndLine()) - 1;
        $methodLines = array_slice($code, $startLine, $endLine - $startLine + 1);

        $annotations = Map {};

        foreach ($methodLines as $line) {
          $matches = array();
          if (preg_match('#/\*\*?\s*@(?P<name>[A-Za-z_-]+)(?:[ \t]+(?P<value>.*?))?[ \t]*\r?\*/$#m', $line, $matches)) {

            $key = strtolower($matches['name']);

            $annotations->set($key, Map {
                'line'  => strval($lineNumber),
                'value' => strval($matches['value'])
            });

          }

          $lineNumber++;
        }

        return $annotations;
    }

    /**
     * @param string $docblock
     *
     * @return array
     *
     * @since Method available since Release 3.4.0
     */
    private static function parseAnnotations(string $docblock): Vector<Annotation> {

        $annotations = Vector {};

        // Strip away the docblock header and footer to ease parsing of one line annotations
        $docblock = substr($docblock, 3, -2);

        $matches = array();
        if (preg_match_all('/@(?P<name>[A-Za-z_-]+)(?:[ \t]+(?P<value>.*?))?[ \t]*\r?$/m', $docblock, $matches)) {
            $numMatches = count($matches[0]);

            for ($i = 0; $i < $numMatches; ++$i) {
              $inst = new Annotation($matches['name'][$i], $matches['value'][$i]);
              $annotations->add($inst);
            }
        }

        return $annotations;
    }

    /**
     * Returns the backup settings for a test.
     *
     * @param string $className
     * @param string $methodName
     *
     * @return array
     *
     * @since Method available since Release 3.4.0
     */
    public static function getBackupSettings(string $className, string $methodName): Map <string, bool> {
      $backupGlobals = self::getBooleanAnnotationSetting(
          $className,
          $methodName,
          'backupGlobals'
      );

      if ( $backupGlobals !== true ) {
        $backupGlobals = false;
      }

      $backupStaticAttributes = self::getBooleanAnnotationSetting(
          $className,
          $methodName,
          'backupStaticAttributes'
      );

      if ( $backupStaticAttributes !== true ) {
        $backupStaticAttributes = false;
      }

      return Map {
        'backupGlobals' => boolval($backupGlobals),
        'backupStaticAttributes' => boolval($backupStaticAttributes)
      };
    }

    /**
     * Returns the dependencies for a test class or method.
     *
     * @param string $className
     * @param string $methodName
     *
     * @return array
     *
     * @since Method available since Release 3.4.0
     */
    public static function getDependencies(string $className, string $methodName): Vector <string>
    {
      return Vector {};
      // JEO Disabling for now, don't think we need the dependency system
      /*
        $annotations = self::parseTestMethodAnnotations(
            $className,
            $methodName
        );

        $dependencies = [];

        if (isset($annotations['class']['depends'])) {
            $dependencies = $annotations['class']['depends'];
        }

        if (isset($annotations['method']['depends'])) {
            $dependencies = array_merge(
                $dependencies,
                $annotations['method']['depends']
            );
        }

        return array_unique($dependencies);
        */
    }

    /**
     * Returns the error handler settings for a test.
     *
     * @param string $className
     * @param string $methodName
     *
     * @return bool
     *
     * @since Method available since Release 3.4.0
     */
    public static function getErrorHandlerSettings(string $className, string $methodName): bool {

      $setting = self::getBooleanAnnotationSetting(
        $className,
        $methodName,
        'errorHandler'
      );

      if ( $setting === true ) {
        return true;
      }

      return false;

    }

    /**
     * Returns the groups for a test class or method.
     *
     * @param string $className
     * @param string $methodName
     *
     * @return array
     *
     * @since Method available since Release 3.2.0
     */
    public static function getGroups(string $className, string $methodName = ''): Vector<string> {
      return Vector {};
      // JEO: Disabling group support.
      /*
        $annotations = self::parseTestMethodAnnotations(
            $className,
            $methodName
        );

        $groups = [];

        if (isset($annotations['method']['author'])) {
            $groups = $annotations['method']['author'];
        } elseif (isset($annotations['class']['author'])) {
            $groups = $annotations['class']['author'];
        }

        if (isset($annotations['class']['group'])) {
            $groups = array_merge($groups, $annotations['class']['group']);
        }

        if (isset($annotations['method']['group'])) {
            $groups = array_merge($groups, $annotations['method']['group']);
        }

        if (isset($annotations['class']['ticket'])) {
            $groups = array_merge($groups, $annotations['class']['ticket']);
        }

        if (isset($annotations['method']['ticket'])) {
            $groups = array_merge($groups, $annotations['method']['ticket']);
        }

        foreach (['method', 'class'] as $element) {
            foreach (['small', 'medium', 'large'] as $size) {
                if (isset($annotations[$element][$size])) {
                    $groups[] = $size;
                    break 2;
                }
            }
        }

        return array_unique($groups);
        */
    }

    /**
     * Returns the size of the test.
     *
     * @param string $className
     * @param string $methodName
     *
     * @return int
     *
     * @since Method available since Release 3.6.0
     */
    public static function getSize(string $className, string $methodName): int {
      // JEO: sizing of tests is no longer dynamic.
      $size   = self::UNKNOWN;
      return $size;
      /*
        $groups = array_flip(self::getGroups($className, $methodName));
        $size   = self::UNKNOWN;
        $class  = new ReflectionClass($className);

        if (isset($groups['large']) ||
            (class_exists('PHPUnit_Extensions_Database_TestCase', false) &&
             $class->isSubclassOf('PHPUnit_Extensions_Database_TestCase'))) {
            $size = self::LARGE;
        } elseif (isset($groups['medium'])) {
            $size = self::MEDIUM;
        } elseif (isset($groups['small'])) {
            $size = self::SMALL;
        }

        return $size;
        */
    }

    /**
     * Returns the tickets for a test class or method.
     *
     * @param string $className
     * @param string $methodName
     *
     * @return array
     *
     * @since Method available since Release 3.4.0
     */
    public static function getTickets(string $className, string $methodName): Vector<string> {
        $annotations = self::parseTestMethodAnnotations(
            $className,
            $methodName
        );

        $tickets = Vector {};

        if ( $annotations->classAnnotations->containsKey('ticket') ) {
          $ticket = $annotations->classAnnotations->get('ticket');
          if ( is_array($ticket) ) {
            foreach ( $ticket as $item ) {
              $tickets->add($item);
            }
          }
        }
        if ( $annotations->methodAnnotations->containsKey('ticket') ) {
          $ticket = $annotations->methodAnnotations->get('ticket');
          if ( is_array($ticket) ) {
            foreach ( $ticket as $item ) {
              $tickets->add($item);
            }
          }
        }

        $tickets = array_unique($tickets->toArray());

        $ticketVec = new Vector($tickets);
        return $ticketVec;

    }

    /**
     * Returns the process isolation settings for a test.
     *
     * @param string $className
     * @param string $methodName
     *
     * @return bool
     *
     * @since Method available since Release 3.4.1
     */
    public static function getProcessIsolationSettings(string $className, string $methodName): bool{
      // JEO: We don't support runTestsInSeperateProcesses on the zynga fork.
      return false;
    }

    /**
     * Returns the preserve global state settings for a test.
     *
     * @param string $className
     * @param string $methodName
     *
     * @return bool
     *
     * @since Method available since Release 3.4.0
     */
    public static function getPreserveGlobalStateSettings(string $className, string $methodName): bool {

      $setting = self::getBooleanAnnotationSetting(
        $className,
        $methodName,
        'preserveGlobalState'
      );

      if ( $setting === true ) {
        return true;
      }

      return false;

    }

    /**
     * @param string $className
     *
     * @return array
     *
     * @since Method available since Release 4.0.8
     */
    public static function getHookMethods(string $className): Map<string, Vector<string>> {
      // We disabled function hooking in the zynga fork.
      return Map {};
      /*
        if (!class_exists($className, false)) {
            return self::emptyHookMethodsArray();
        }

        if (!isset(self::$hookMethods[$className])) {
            self::$hookMethods[$className] = self::emptyHookMethodsArray();

            try {
                $class = new ReflectionClass($className);

                foreach ($class->getMethods() as $method) {
                    if (self::isBeforeClassMethod($method)) {
                        self::$hookMethods[$className]['beforeClass'][] = $method->getName();
                    }

                    if (self::isBeforeMethod($method)) {
                        self::$hookMethods[$className]['before'][] = $method->getName();
                    }

                    if (self::isAfterMethod($method)) {
                        self::$hookMethods[$className]['after'][] = $method->getName();
                    }

                    if (self::isAfterClassMethod($method)) {
                        self::$hookMethods[$className]['afterClass'][] = $method->getName();
                    }
                }
            } catch (ReflectionException $e) {
            }
        }

        return self::$hookMethods[$className];
        */
    }

    /**
     * @return array
     *
     * @since Method available since Release 4.0.9
     */
    private static function emptyHookMethodsArray(): Map<string, Vector<string>> {
        return Map {
            'beforeClass' => Vector {'setUpBeforeClass'},
            'before'      => Vector {'setUp'},
            'after'       => Vector {'tearDown'},
            'afterClass'  => Vector {'tearDownAfterClass'}
        };
    }

    /**
     * @param string $className
     * @param string $methodName
     * @param string $settingName
     *
     * @return bool
     *
     * @since Method available since Release 3.4.0
     */
    private static function getBooleanAnnotationSetting(string $className, string $methodName, string $settingName): ?bool {

        $annotations = self::parseTestMethodAnnotations(
            $className,
            $methodName
        );

        $result = null;

        if ( $annotations->classAnnotations->containsKey($settingName) ) {
          $settingValue = $annotations->classAnnotations->get($settingName);
          if ($settingValue == 'enabled') {
            $result = true;
          } elseif ($settingValue == 'disabled') {
            $result = false;
          }
        }

        if ($annotations->methodAnnotations->containsKey($settingName)) {
          $settingValue = $annotations->methodAnnotations->get($settingName);
          if ($settingValue == 'enabled') {
            $result = true;
          } elseif ($settingValue == 'disabled') {
            $result = false;
          }
        }

        return $result;
    }

    /**
     * @param string $element
     *
     * @return array
     *
     * @throws InvalidCoversTargetException
     *
     * @since Method available since Release 4.0.0
     */
    private static function resolveElementToReflectionObjects(string $element): Vector<mixed> {
        $codeToCoverList = Vector {};

        if (strpos($element, '\\') !== false && function_exists($element)) {
          $reflected = ReflectionFunctions::getReflection($element);
          $codeToCoverList->add($reflected);
        } elseif (strpos($element, '::') !== false) {

            list($className, $methodName) = explode('::', $element);

            if ( strpos($methodName, '<') == 0 ) {

                $classes = [$className];

                foreach ($classes as $className) {
                  if ( Reflectable::isClassReflectable($className) !== true ) {
                    throw new InvalidCoversTargetException(
                      sprintf(
                        'Trying to @cover or @use not existing class or ' .
                        'interface "%s".',
                        $className
                      )
                    );
                  }

                  $methods = ReflectionMethods::getAllReflections($className);

                  if ( $methods === null ) {
                    return $codeToCoverList;
                  }

                  $inverse = false;

                  if ( strpos($methodName, '!') == 0 ) {
                    $inverse = true;
                  }

                  $isProtected = false;
                  $isPublic = false;
                  $isPrivate = false;

                  if (strpos($methodName, 'protected')) {
                    $isProtected = true;
                  } elseif (strpos($methodName, 'private')) {
                    $isPrivate = true;
                  } elseif (strpos($methodName, 'public')) {
                    $isPublic = true;
                  }

                  foreach ($methods as $method) {
                    if ($inverse && $isPublic && ! $method->isPublic()) {
                      $codeToCoverList->add($method);
                    } elseif ($inverse && $isPrivate && ! $method->isPrivate()) {
                      $codeToCoverList->add($method);
                    } elseif ($inverse && $isProtected && ! $method->isProtected()) {
                      $codeToCoverList->add($method);
                    } elseif (!$inverse && $isPublic && $method->isPublic()) {
                      $codeToCoverList->add($method);
                    } elseif (!$inverse && $isPrivate && $method->isPrivate()) {
                      $codeToCoverList->add($method);
                    } elseif (!$inverse && $isProtected && $method->isProtected()) {
                      $codeToCoverList->add($method);
                    }
                  }
                }
            } else {

              $classes = Vector {};
              $classes->add($className);

              foreach ($classes as $className) {
                if ($className == '' ) {

                  $function = ReflectionFunctions::getReflection($methodName);

                  if ( $function instanceof ReflectionFunction ) {
                    $codeToCoverList->add($function);
                  }

                } else {

                  if ( Reflectable::isClassReflectable($className) !== true ||
                       Reflectable::isMethodReflectable($className, $methodName) !== true ) {
                    throw new InvalidCoversTargetException(
                      sprintf(
                        'Trying to @cover or @use not existing method "%s::%s".',
                        $className,
                        $methodName
                        )
                    );
                  }

                  $method = ReflectionMethods::getReflection($className, $methodName);

                  $codeToCoverList->add($method);
                }
              }
          }
        } else {
            $extended = false;

            if (strpos($element, '<extended>') !== false) {
                $element  = str_replace('<extended>', '', $element);
                $extended = true;
            }

            $classes = [$element];

            if ($extended) {
                $classes = array_merge(
                    $classes,
                    class_implements($element),
                    class_parents($element)
                );
            }

            foreach ($classes as $className) {
                if (!class_exists($className) &&
                    !interface_exists($className) &&
                    !trait_exists($className)) {
                    throw new InvalidCoversTargetException(
                        sprintf(
                            'Trying to @cover or @use not existing class or ' .
                            'interface "%s".',
                            $className
                        )
                    );
                }

                $reflectedClass = ReflectionClasses::getReflection($className);

                if ( $reflectedClass instanceof ReflectionClass ) {
                  $codeToCoverList->add($reflectedClass);
                }

            }
        }

      return $codeToCoverList;

    }

    private static function _resolveReflectorIntoCodeAttributes(mixed $reflector): (string, int, int) {

      $fileName = '';
      $startLine = 0;
      $endLine = 0;

      if ( $reflector instanceof ReflectionClass  ) {
        $fileName = strval($reflector->getFileName());
        $startLine = intval($reflector->getStartLine());
        $endLine = intval($reflector->getEndLine());
      } elseif ( $reflector instanceof ReflectionMethod ) {
        $fileName = strval($reflector->getFileName());
        $startLine = intval($reflector->getStartLine());
        $endLine = intval($reflector->getEndLine());
      } elseif ( $reflector instanceof ReflectionFunction ) {
        $fileName = strval($reflector->getFileName());
        $startLine = intval($reflector->getStartLine());
        $endLine = intval($reflector->getEndLine());
      }
      return tuple($fileName, $startLine, $endLine);
    }
    /**
     * @param array $reflectors
     *
     * @return array
     */
    private static function resolveReflectionObjectsToLines(Vector<mixed> $reflectors): Map<string, Vector<int>> {

        $result = Map {};

        foreach ($reflectors as $reflector) {

          list($fileName, $startLine, $endLine) = self::_resolveReflectorIntoCodeAttributes(
            $reflector
          );

          $t_range = Vector {};

          if ( $result->containsKey($fileName) ) {
            $existing_range = $result->get($fileName);
            if ( $existing_range instanceof Vector ) {
              $t_range = $existing_range;
            }
          }

          $lineRange = range($startLine, $endLine);

          foreach ( $lineRange as $line ) {
            if ( $t_range instanceof Vector ) {
              if ($t_range->linearSearch($line) >= 0) {
                $t_range->add($line);
              }
            }
          }

          $result->set($fileName, $t_range);

        }

        return $result;
    }

    /**
     * @param ReflectionMethod $method
     *
     * @return bool
     *
     * @since Method available since Release 4.0.8
     */
    private static function isBeforeClassMethod(ReflectionMethod $method): bool {
        return $method->isStatic() && strpos($method->getDocComment(), '@beforeClass') !== false;
    }

    /**
     * @param ReflectionMethod $method
     *
     * @return bool
     *
     * @since Method available since Release 4.0.8
     */
    private static function isBeforeMethod(ReflectionMethod $method): bool {
      if ( preg_match('/@before\b/', $method->getDocComment()) ) {
        return true;
      }
      return false;
    }

    /**
     * @param ReflectionMethod $method
     *
     * @return bool
     *
     * @since Method available since Release 4.0.8
     */
    private static function isAfterClassMethod(ReflectionMethod $method): bool {
        return
          $method->isStatic() &&
          strpos($method->getDocComment(), '@afterClass') !== false;
    }

    /**
     * @param ReflectionMethod $method
     *
     * @return bool
     *
     * @since Method available since Release 4.0.8
     */
    private static function isAfterMethod(ReflectionMethod $method): bool {
        if (preg_match('/@after\b/', $method->getDocComment())) {
          return true;
        }
        return false;
    }
}
