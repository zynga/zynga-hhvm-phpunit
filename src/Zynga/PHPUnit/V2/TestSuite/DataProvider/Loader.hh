<?hh // strict

namespace Zynga\PHPUnit\V2\TestSuite\DataProvider;

use Zynga\PHPUnit\V2\Exceptions\Exception as PHPUnitException;

use Zynga\Framework\ReflectionCache\V1\ReflectionClasses;
//use Zynga\Framework\ReflectionCache\V1\ReflectionFunctions;
use Zynga\Framework\ReflectionCache\V1\ReflectionMethods;
// use Zynga\PHPUnit\V2\Interfaces\TestInterface;
// use Zynga\PHPUnit\V2\TestCase;

use \Exception;
use \ReflectionClass;
use \ReflectionMethod;

class Loader {

  const
    REGEX_DATA_PROVIDER = '/@dataProvider\s+([a-zA-Z0-9._:-\\\\x7f-\xff]+)/';
  const REGEX_TEST_WITH = '/@testWith\s+/';

  /**
   * Returns the provided data for a method.
   *
   * @param string $className
   * @param string $methodName
   *
   * @return array|Iterator when a data provider is specified and exists
   *                        null           when no data provider is specified
   *
   * @throws PHPUnit_Framework_Exception
   *
   * @since Method available since Release 3.2.0
   */
  public static function getProvidedData(
    string $className,
    string $methodName,
  ): mixed {

    $reflector = ReflectionMethods::getReflection($className, $methodName);

    if (!$reflector instanceof ReflectionMethod) {
      throw new Exception(
        'getProvidedData failed to reflect className='.
        $className.
        ' methodName='.
        $methodName,
      );
    }

    $docComment = $reflector->getDocComment();
    $data = null;

    if ($dataProviderData = self::getDataFromDataProviderAnnotation(
          strval($docComment),
          $className,
          $methodName,
        )) {
      $data = $dataProviderData;
    }

    if ($testWithData =
          self::getDataFromTestWithAnnotation(strval($docComment))) {
      $data = $testWithData;
    }

    if (is_object($data)) {
      $data = iterator_to_array($data);
    }

    if (is_array($data)) {

      foreach ($data as $key => $value) {
        if (!is_array($value)) {
          throw new PHPUnitException(
            sprintf(
              'Data set %s is invalid.',
              is_int($key) ? '#'.$key : '"'.$key.'"',
            ),
          );
        }
      }
    }

    return $data;
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
   * @throws PHPUnit_Framework_Exception
   */
  private static function getDataFromDataProviderAnnotation(
    string $docComment,
    string $className,
    string $methodName,
  ): mixed {

    $matches = array();

    if (preg_match(self::REGEX_DATA_PROVIDER, $docComment, $matches)) {

      $dataProviderMethodNameNamespace = explode('\\', $matches[1]);
      $leaf = explode('::', array_pop($dataProviderMethodNameNamespace));
      $dataProviderMethodName = array_pop($leaf);

      if ($dataProviderMethodNameNamespace != '') {
        $dataProviderMethodNameNamespace =
          implode('\\', $dataProviderMethodNameNamespace).'\\';
      } else {
        $dataProviderMethodNameNamespace = '';
      }

      if ($leaf != '') {
        $dataProviderClassName =
          $dataProviderMethodNameNamespace.array_pop($leaf);
      } else {
        $dataProviderClassName = $className;
      }

      $dataProviderClass =
        ReflectionClasses::getReflection($dataProviderClassName);

      if (!$dataProviderClass instanceof ReflectionClass) {
        throw new Exception(
          'Failed to reflect dataProviderClassName='.$dataProviderClassName,
        );
      }

      $dataProviderMethod =
        $dataProviderClass->getMethod($dataProviderMethodName);

      if ($dataProviderMethod->isStatic()) {
        $object = null;
      } else {
        $object = $dataProviderClass->newInstance($methodName);
      }

      if ($dataProviderMethod->getNumberOfParameters() == 0) {
        $data = $dataProviderMethod->invoke($object);
      } else {
        $data = $dataProviderMethod->invoke($object, $methodName);
      }

      return $data;
    }
  }

  /**
   * @param string $docComment full docComment string
   *
   * @return array when @testWith annotation is defined
   *               null  when @testWith annotation is omitted
   *
   * @throws PHPUnit_Framework_Exception when @testWith annotation is defined but cannot be parsed
   */
  public static function getDataFromTestWithAnnotation(
    string $docComment,
  ): mixed {

    $docComment = self::cleanUpMultiLineAnnotation($docComment);

    $matches = array();

    if (preg_match(
          self::REGEX_TEST_WITH,
          $docComment,
          $matches,
          PREG_OFFSET_CAPTURE,
        )) {
      $offset = strlen($matches[0][0]) + $matches[0][1];
      $annotationContent = substr($docComment, $offset);
      $data = [];
      foreach (explode("\n", $annotationContent) as $candidateRow) {
        $candidateRow = trim($candidateRow);
        $dataSet = json_decode($candidateRow, true);
        if (json_last_error() != JSON_ERROR_NONE) {
          break;
        }
        $data[] = $dataSet;
      }

      if (!$data) {
        throw new PHPUnitException(
          'The dataset for the @testWith annotation cannot be parsed.',
        );
      }

      return $data;
    }

    return null;

  }

  private static function cleanUpMultiLineAnnotation(
    string $docComment,
  ): string {

    //removing initial '   * ' for docComment
    $docComment =
      preg_replace('/'.'\n'.'\s*'.'\*'.'\s?'.'/', "\n", $docComment);
    $docComment = substr($docComment, 0, -1);
    $docComment = rtrim($docComment, "\n");

    return $docComment;

  }

}
