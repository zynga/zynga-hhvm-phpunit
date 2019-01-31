<?hh // strict

namespace SebastianBergmann\PHPUnit\Environment;

class LanguageTypes {
  const TYPE_ARRAY = 'array';
  const TYPE_BOOL = 'bool';
  const TYPE_BOOL_ALIAS = 'boolean';
  const TYPE_FLOAT = 'float';
  const TYPE_DOUBLE = 'double';
  const TYPE_INT = 'int';
  const TYPE_INT_ALIAS = 'integer';
  const TYPE_NULL = 'null';
  const TYPE_NUMERIC = 'numeric';
  const TYPE_OBJECT = 'object';
  const TYPE_RESOURCE = 'resource';
  const TYPE_STRING = 'string';
  const TYPE_SCALAR = 'scalar';
  const TYPE_CALLABLE = 'callable';

  private static Vector<string>
    $nativeTypes = Vector {
      self::TYPE_ARRAY,
      self::TYPE_BOOL,
      self::TYPE_BOOL_ALIAS,
      self::TYPE_DOUBLE,
      self::TYPE_FLOAT,
      self::TYPE_INT,
      self::TYPE_INT_ALIAS,
      self::TYPE_NULL,
      self::TYPE_NUMERIC,
      self::TYPE_OBJECT,
      self::TYPE_RESOURCE,
      self::TYPE_STRING,
      self::TYPE_SCALAR,
      self::TYPE_CALLABLE,
    };

  public static function isNativeType(string $type): bool {
    return in_array($type, self::$nativeTypes);
  }

}
