<?hh // strict

namespace SebastianBergmann\TokenStream\Token;

class CustomTokens {

  // '`' => 'PHP_Token_Backtick', 96
  //
  private static ImmMap<int, string>
    $_idToClassName = ImmMap {
      33 => 'PHP_Token_Exclamation_Mark',
      34 => 'PHP_Token_Double_Quotes',
      36 => 'PHP_Token_Dollar',
      37 => 'PHP_Token_Percent',
      38 => 'PHP_Token_Ampersand',
      40 => 'PHP_Token_Open_Bracket',
      41 => 'PHP_Token_Close_Bracket',
      42 => 'PHP_Token_Mult',
      43 => 'PHP_Token_Plus',
      44 => 'PHP_Token_Comma',
      45 => 'PHP_Token_Minus',
      46 => 'PHP_Token_Dot',
      47 => 'PHP_Token_Div',
      58 => 'PHP_Token_Colon',
      59 => 'PHP_Token_Semicolon',
      60 => 'PHP_Token_Lt',
      61 => 'PHP_Token_Equal',
      62 => 'PHP_Token_Gt',
      63 => 'PHP_Token_Question_Mark',
      64 => 'PHP_Token_At',
      91 => 'PHP_Token_Open_Square',
      93 => 'PHP_Token_Close_Square',
      94 => 'PHP_Token_Caret',
      96 => 'PHP_Token_Backtick',
      123 => 'PHP_Token_Open_Curly',
      124 => 'PHP_Token_Pipe',
      125 => 'PHP_Token_Close_Curly',
      126 => 'PHP_Token_Tilde',
    };

  private static ImmMap<string, int>
    $_tokenToId = ImmMap {
      '!' => 33,
      '"' => 34,
      '%' => 37,
      '$' => 36,
      '&' => 38,
      '(' => 40,
      ')' => 41,
      '+' => 43,
      '*' => 42,
      ',' => 44,
      '-' => 45,
      '.' => 46,
      '/' => 47,
      ':' => 58,
      ';' => 59,
      '<' => 60,
      '=' => 61,
      '>' => 62,
      '?' => 63,
      '@' => 64,
      '[' => 91,
      ']' => 93,
      '^' => 94,
      '`' => 96,
      '{' => 123,
      '|' => 124,
      '}' => 125,
      '~' => 126,
    };

  public static function getTokenClassNameFromId(int $id): ?string {

    $className = self::$_idToClassName->get($id);

    if ($className !== null) {
      return $className;
    }

    return null;

  }

  public static function getTokenIdFromString(string $value): int {

    if ($value == '!') {
      return 33;
    }
    if ($value == '"') {
      return 34;
    }
    if ($value == '$') {
      return 36;
    }
    if ($value == '%') {
      return 37;
    }
    if ($value == '&') {
      return 38;
    }
    if ($value == '(') {
      return 40;
    }
    if ($value == ')') {
      return 41;
    }
    if ($value == '*') {
      return 42;
    }
    if ($value == '+') {
      return 43;
    }
    if ($value == ',') {
      return 44;
    }
    if ($value == '-') {
      return 45;
    }
    if ($value == '.') {
      return 46;
    }
    if ($value == '/') {
      return 47;
    }
    if ($value == ':') {
      return 58;
    }
    if ($value == ';') {
      return 59;
    }
    if ($value == '<') {
      return 60;
    }
    if ($value == '=') {
      return 61;
    }
    if ($value == '>') {
      return 62;
    }
    if ($value == '?') {
      return 63;
    }
    if ($value == '@') {
      return 64;
    }
    if ($value == '[') {
      return 91;
    }
    if ($value == ']') {
      return 93;
    }
    if ($value == '^') {
      return 94;
    }
    if ($value == '`') {
      return 96;
    }
    if ($value == '{') {
      return 123;
    }
    if ($value == '|') {
      return 124;
    }
    if ($value == '}') {
      return 125;
    }
    if ($value == '~') {
      return 126;
    }

    //echo "miss=$value\n";
    return -1;

  }

}
