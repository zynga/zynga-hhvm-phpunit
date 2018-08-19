<?hh // strict

namespace SebastianBergmann\TokenStream\Token\Stream;

use SebastianBergmann\TokenStream\Token\Factory as TokenFactory;

use Zynga\Framework\Dynamic\V1\DynamicClassCreation;
use SebastianBergmann\TokenStream\Token\Stream;
use SebastianBergmann\TokenStream\TokenInterface;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Comment;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Doc_Comment;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Halt_Compiler;

use Zynga\CodeBase\V1\File;
use Zynga\CodeBase\V1\File\RawToken;

use \Exception;

class Scanner {

  protected static Map<int, string> $tokenNameCache = Map {};

  /**
   * @var array
   */
  protected static Map<string, string>
    $customTokens = Map {
      '(' => 'PHP_Token_Open_Bracket',
      ')' => 'PHP_Token_Close_Bracket',
      '[' => 'PHP_Token_Open_Square',
      ']' => 'PHP_Token_Close_Square',
      '{' => 'PHP_Token_Open_Curly',
      '}' => 'PHP_Token_Close_Curly',
      ';' => 'PHP_Token_Semicolon',
      '.' => 'PHP_Token_Dot',
      ',' => 'PHP_Token_Comma',
      '=' => 'PHP_Token_Equal',
      '<' => 'PHP_Token_Lt',
      '>' => 'PHP_Token_Gt',
      '+' => 'PHP_Token_Plus',
      '-' => 'PHP_Token_Minus',
      '*' => 'PHP_Token_Mult',
      '/' => 'PHP_Token_Div',
      '?' => 'PHP_Token_Question_Mark',
      '!' => 'PHP_Token_Exclamation_Mark',
      ':' => 'PHP_Token_Colon',
      '"' => 'PHP_Token_Double_Quotes',
      '@' => 'PHP_Token_At',
      '&' => 'PHP_Token_Ampersand',
      '%' => 'PHP_Token_Percent',
      '|' => 'PHP_Token_Pipe',
      '$' => 'PHP_Token_Dollar',
      '^' => 'PHP_Token_Caret',
      '~' => 'PHP_Token_Tilde',
      '`' => 'PHP_Token_Backtick',
    };

  /**
   * Scans the source for sequences of characters and converts them into a
   * stream of tokens.
   */
  public function scan(File $codeFile, Stream $stream): void {

    if ($codeFile->source()->load() !== true ||
        $codeFile->rawTokens()->load() !== true) {
      return;
    }

    $sourceCode = $codeFile->source()->get();
    $tokens = $codeFile->rawTokens()->get();

    $id = 0;
    $line = 1;

    $numTokens = $tokens->count();

    $lastNonWhitespaceTokenWasDoubleColon = false;
    $name = '';

    for ($i = 0; $i < $numTokens; ++$i) {

      $token = $tokens->get($i);

      $tokenClass = '';
      $text = '';
      $skip = 0;

      if (!$token instanceof RawToken) {
        continue;
      }

      if ($token->getTokenId() === -1) {
        $text = $token->getText();
        $t_tokenClass = self::$customTokens->get($text);

        if (is_string($t_tokenClass)) {
          $tokenClass = $t_tokenClass;
        }

      }

      if ($tokenClass == '') {

        $name = substr($this->resolveTokenName($token->getTokenId()), 2);

        $text = $token->getText();

        if ($lastNonWhitespaceTokenWasDoubleColon && $name == 'CLASS') {
          $name = 'CLASS_NAME_CONSTANT';
        } else if ($name == 'USE' &&
                   $tokens->containsKey($i + 2) &&
                   $tokens[$i + 2]->getTokenId() == T_FUNCTION) {
          $name = 'Use_Function';
          $skip = 2;
        }

        // convert the raw token name into a class name
        $nameParts = explode("_", $name);

        $name = '';
        foreach ($nameParts as $namePart) {
          if ($name != '') {
            $name .= '_';
          }
          $name .= ucfirst(strtolower($namePart));
        }

        $tokenClass = 'PHP_Token_'.$name;

      }

      $newToken =
        TokenFactory::createToken($tokenClass, $text, $line, $codeFile, $id);

      if ($newToken === null) {
        // --
        // JEO: DynamicClassCreation is actually somewhat slow with the number
        // of ops we are asking it to do.
        // --
        $tokenParams = Vector {$text, $line, $codeFile, $id};

        $newToken = DynamicClassCreation::createClassByNameGeneric(
          'SebastianBergmann\TokenStream\Tokens\\'.$tokenClass,
          $tokenParams,
        );

      }
      $id++;

      if ($newToken instanceof TokenInterface) {
        $stream->tokens()->add($newToken);
      }

      $lines = substr_count($text, "\n");
      $line += $lines;

      if ($newToken instanceof PHP_Token_Halt_Compiler) {
        break;
      } else if ($newToken instanceof PHP_Token_Comment ||
                 $newToken instanceof PHP_Token_Doc_Comment) {
        // @TODO: This should move up to the parent file structure anyways.
        //        $this->linesOfCode['cloc'] += $lines + 1;
      }

      if ($name == 'DOUBLE_COLON') {
        $lastNonWhitespaceTokenWasDoubleColon = true;
      } else if ($name != 'WHITESPACE') {
        $lastNonWhitespaceTokenWasDoubleColon = false;
      }

      $i += $skip;
    }

    // @TODO, these should move up to the file structure anywasy.
    //    $this->linesOfCode['loc'] = substr_count($sourceCode, "\n");
    //    $this->linesOfCode['ncloc'] =
    //      $this->linesOfCode['loc'] - $this->linesOfCode['cloc'];

  }

  protected function resolveTokenName(int $tokenId): string {

    $cachedTokenName = self::$tokenNameCache->get($tokenId);

    if (is_string($cachedTokenName)) {
      return $cachedTokenName;
    }

    $tokenName = token_name($tokenId);

    if (!is_string($tokenName)) {
      throw new Exception('tokenId='.$tokenId.' - is unknown to token_name');
    }

    self::$tokenNameCache->set($tokenId, $tokenName);

    return $tokenName;

  }

}
