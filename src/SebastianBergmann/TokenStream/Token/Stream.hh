<?hh // strict

namespace SebastianBergmann\TokenStream\Token;

/*
 * This file is part of php-token-stream.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use Zynga\Source\Cache as Zynga_Source_Cache;
use Zynga\Framework\Dynamic\V1\DynamicClassCreation;

use SebastianBergmann\TokenStream\Token as PHP_Token;
use SebastianBergmann\TokenStream\Token\Factory as TokenFactory;

use SebastianBergmann\TokenStream\TokenInterface;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Class;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Close_Curly;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Comment;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Doc_Comment;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Function;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Include;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Include_Once;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Halt_Compiler;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Interface;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Require;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Require_Once;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Trait;

use \Exception;
use \OutOfBoundsException;

use Zynga\CodeBase\V1\FileFactory;
use Zynga\CodeBase\V1\Code\Code_Class;
use Zynga\CodeBase\V1\Code\Code_Method;

/**
 * A stream of PHP tokens.
 */
class Stream {

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

  protected bool $didScan;

  protected bool $didParse;

  /**
   * @var string
   */
  protected string $filename;

  /**
   * @var array
   */
  protected Vector<TokenInterface> $tokens;

  /**
   * @var int
   */
  protected int $position;

  /**
   * @var array
   */
  protected Map<string, int>
    $linesOfCode = Map {'loc' => 0, 'cloc' => 0, 'ncloc' => 0};

  /**
   * @var array
   */
  protected Map<string, Vector<string>>
    $includes = Map {
      'require_once' => Vector {},
      'require' => Vector {},
      'include_once' => Vector {},
      'include' => Vector {},
    };

  /**
   * @var array
   */
  protected Map<string, StreamInterfaceStructure> $interfaces;

  /**
   * @var array
   */
  protected Map<int, string> $lineToFunctionMap;

  // jeo: made temporary variables to avoid parameter passing throughout this
  //   code as it's refactored.
  private Vector<string> $t_class;
  private Vector<int> $t_classEndLine;
  private mixed $t_trait;
  private mixed $t_traitEndLine;
  private mixed $t_interface;
  private mixed $t_interfaceEndLine;
  private int $streamId;

  /**
   * Constructor.
   *
   * @param string $fileName
   */
  public function __construct(string $fileName, int $streamId = -1) {

    $this->didScan = false;
    $this->didParse = false;
    $this->tokens = Vector {};
    $this->interfaces = Map {};
    $this->lineToFunctionMap = Map {};
    $this->position = 0;

    $this->t_class = Vector {};
    $this->t_classEndLine = Vector {};
    $this->t_trait = false;
    $this->t_traitEndLine = false;
    $this->t_interface = false;
    $this->t_interfaceEndLine = false;

    $this->filename = $fileName;
    $this->streamId = $streamId;

  }

  public function getStreamId(): int {
    return $this->streamId;
  }

  public function get(int $offset): ?TokenInterface {
    return $this->tokens->get($offset);
  }

  /**
   * Destructor.
   */
  public function __destruct() {
    $this->tokens->clear();
  }

  /**
   * @return string
   */
  public function __toString(): string {
    $buffer = '';

    foreach ($this->tokens as $token) {
      $buffer .= $token;
    }

    return $buffer;
  }

  /**
   * @return string
   */
  public function getFilename(): string {
    return $this->filename;
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

  /**
   * Scans the source for sequences of characters and converts them into a
   * stream of tokens.
   */
  private function scan(): void {

    if ($this->didScan === true) {
      return;
    }

    if (!is_file($this->filename)) {
      $this->didScan = true;
      return;
    }

    $sourceCode = Zynga_Source_Cache::getSource($this->filename);

    $id = 0;
    $line = 1;

    $tokens = Zynga_Source_Cache::getTokens($this->filename);

    if (!is_array($tokens)) {
      return;
    }

    $numTokens = count($tokens);

    $lastNonWhitespaceTokenWasDoubleColon = false;
    $name = '';

    for ($i = 0; $i < $numTokens; ++$i) {
      $token = $tokens[$i];
      $skip = 0;

      if (is_array($token)) {

        $name = substr($this->resolveTokenName($token[0]), 2);

        $text = $token[1];

        if ($lastNonWhitespaceTokenWasDoubleColon && $name == 'CLASS') {
          $name = 'CLASS_NAME_CONSTANT';
        } else if ($name == 'USE' &&
                   array_key_exists($i + 2, $tokens) &&
                   is_array($tokens[$i + 2]) &&
                   $tokens[$i + 2][0] == T_FUNCTION) {
          $name = 'USE_FUNCTION';
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

      } else {
        $text = $token;
        $tokenClass = self::$customTokens[$token];
      }

      $newToken = TokenFactory::createToken(
        $tokenClass,
        $text,
        $line,
        $this->getStreamId(),
        $id,
      );

      if ($newToken === null) {
        // --
        // JEO: DynamicClassCreation is actually somewhat slow with the number
        // of ops we are asking it to do.
        // --
        $tokenParams = Vector {$text, $line, $this->getStreamId(), $id};

        $newToken = DynamicClassCreation::createClassByNameGeneric(
          'SebastianBergmann\TokenStream\Tokens\\'.$tokenClass,
          $tokenParams,
        );
      }
      $id++;

      if ($newToken instanceof TokenInterface) {
        $this->tokens->add($newToken);
      }

      $lines = substr_count($text, "\n");
      $line += $lines;

      if ($newToken instanceof PHP_Token_Halt_Compiler) {
        break;
      } else if ($newToken instanceof PHP_Token_Comment ||
                 $newToken instanceof PHP_Token_Doc_Comment) {
        $this->linesOfCode['cloc'] += $lines + 1;
      }

      if ($name == 'DOUBLE_COLON') {
        $lastNonWhitespaceTokenWasDoubleColon = true;
      } else if ($name != 'WHITESPACE') {
        $lastNonWhitespaceTokenWasDoubleColon = false;
      }

      $i += $skip;
    }

    $this->linesOfCode['loc'] = substr_count($sourceCode, "\n");
    $this->linesOfCode['ncloc'] =
      $this->linesOfCode['loc'] - $this->linesOfCode['cloc'];

    $this->didScan = true;

  }

  public function getLineCount(): int {
    $locEntry = $this->linesOfCode->get('loc');
    if (is_int($locEntry)) {
      return $locEntry;
    }
    return -1;
  }

  /**
   * @return int
   */
  public function count(): int {
    $this->scan();
    return $this->tokens->count();
  }

  /**
   * @return PHP_Token[]
   */
  public function tokens(): Vector<TokenInterface> {
    $this->scan();
    return $this->tokens;
  }

  /**
   * @return array
   */
  public function getInterfaces(): Map<string, StreamInterfaceStructure> {
    $this->parse();
    return $this->interfaces;
  }

  /**
   * Gets the names of all files that have been included
   * using include(), include_once(), require() or require_once().
   *
   * Parameter $categorize set to TRUE causing this function to return a
   * multi-dimensional array with categories in the keys of the first dimension
   * and constants and their values in the second dimension.
   *
   * Parameter $category allow to filter following specific inclusion type
   *
   * @param bool   $categorize OPTIONAL (true Returns Map<string, Vector<string>>, false returns Vector<string>)
   * @param string $category   OPTIONAL Either 'require_once', 'require',
   *                           'include_once', 'include'.
   *
   * @return array
   */
  public function getIncludes(
    bool $categorize = false,
    ?string $category = null,
  ): mixed {

    $this->parse();

    if ($category !== null &&
        $this->includes->containsKey($category) === true) {
      $includes = $this->includes->get($category);
      return $includes;
    }

    if ($categorize === false) {
      $return = Vector {};
      $return->addAll($this->includes->get('require_once'));
      $return->addAll($this->includes->get('require'));
      $return->addAll($this->includes->get('include_once'));
      $return->addAll($this->includes->get('include'));
      return $return;
    }

    return $this->includes;

  }

  /**
   * Returns the name of the function or method a line belongs to.
   *
   * @return string or null if the line is not in a function or method
   */
  public function getFunctionForLine(int $line): ?string {
    $this->parse();
    return $this->lineToFunctionMap->get($line);

  }

  private function clearParseBuffers(): void {
    $this->t_classEndLine->clear();
    $this->t_class->clear();
    // JEO: this flop from boolean to the 'current name of the attribute'
    $this->t_trait = false;
    $this->t_traitEndLine = false;
    $this->t_interface = false;
    $this->t_interfaceEndLine = false;
  }

  private function parseHandleInterface(PHP_Token_Interface $token): void {

    $interface = $token->getName();
    $interfaceEndLine = $token->getEndLine();

    $tmp = new StreamInterfaceStructure();
    $tmp->methods->clear();
    $tmp->parent = $token->getParent();
    $tmp->keywords = $token->getKeywords();
    $tmp->docblock = strval($token->getDocblock());
    $tmp->startLine = $token->getLine();
    $tmp->endLine = $interfaceEndLine;
    $tmp->package = $token->getPackage();
    $tmp->file = $this->filename;

    $this->interfaces->set($interface, $tmp);
    $this->t_interface = $interface;

  }

  private function parseHandleClass(PHP_Token_Class $token): void {

    $codeFile = FileFactory::get($this->filename);

    $tmp = new Code_Class();
    $tmp->methods->clear();
    $tmp->parent = $token->getParent();
    $tmp->interfaces = $token->getInterfaces();
    $tmp->keywords = $token->getKeywords();
    $tmp->docblock = strval($token->getDocblock());
    $tmp->startLine = $token->getLine();
    $tmp->endLine = $token->getEndLine();
    $tmp->package = $token->getPackage();
    $tmp->file = $this->filename;

    $this->t_class->add($token->getName());

    $this->t_classEndLine->add($token->getEndLine());

    $previousClass = $this->t_class->get($this->t_class->count() - 1);

    if (is_string($previousClass) && $previousClass != 'anonymous class') {
      $codeFile->classes()->add($previousClass, $tmp);
    }

  }

  private function parseHandleTrait(PHP_Token_Trait $token): void {

    $codeFile = FileFactory::get($this->filename);
    $tmp = new Code_Class();
    $tmp->methods->clear();
    $tmp->parent = $token->getParent();
    $tmp->interfaces = $token->getInterfaces();
    $tmp->keywords = $token->getKeywords();
    $tmp->docblock = strval($token->getDocblock());
    $tmp->startLine = $token->getLine();
    $tmp->endLine = $token->getEndLine();
    $tmp->package = $token->getPackage();
    $tmp->file = $this->filename;

    $this->t_trait = $token->getName();
    $this->t_traitEndLine = $token->getEndLine();

    $codeFile->traits()->add($token->getName(), $tmp);

  }

  private function parseHandleFunction(PHP_Token_Function $token): void {

    $codeFile = FileFactory::get($this->filename);

    $name = $token->getName();

    $tmp = new Code_Method();

    $tmp->methodName = $token->getName();
    $tmp->docblock = strval($token->getDocblock());
    $tmp->keywords = $token->getKeywords();
    $tmp->visibility = $token->getVisibility();
    $tmp->signature = $token->getSignature();
    $tmp->startLine = $token->getLine();
    $tmp->endLine = $token->getEndLine();
    $tmp->ccn = $token->getCCN();
    $tmp->file = $this->filename;

    if ($this->t_class->count() == 0 &&
        $this->t_trait === false &&
        $this->t_interface === false) {

      $codeFile->functions()->add($name, $tmp);

      $this->addFunctionToMap($name, $tmp->startLine, $tmp->endLine);

    } else if ($this->t_class->count() != 0 &&
               strval($this->t_class->get($this->t_class->count() - 1)) != 'anonymous class') {

      $a_className = $this->t_class->get($this->t_class->count() - 1);

      $a_class = null;
      if (is_string($a_className)) {
        $a_class = $codeFile->classes()->get($a_className);
      }

      if ($a_class instanceof Code_Class) {
        $a_class->methods->set($name, $tmp);

        $this->addFunctionToMap(
          $a_className.'::'.$name,
          $tmp->startLine,
          $tmp->endLine,
        );

      }

    } else if ($this->t_trait !== false) {

      $a_traitname = strval($this->t_trait);
      $a_trait = null;

      $a_trait = $codeFile->traits()->get($a_traitname);

      if ($a_trait instanceof Code_Class) {

        $a_trait->methods->set($name, $tmp);

        $this->addFunctionToMap(
          $a_traitname.'::'.$name,
          $tmp->startLine,
          $tmp->endLine,
        );
      }

    } else {

      $a_interfacename = strval($this->t_interface);
      $a_interface = $this->interfaces->get($a_interfacename);
      if ($a_interface instanceof StreamInterfaceStructure) {
        $a_interface->methods->set($name, $tmp);
      }
    }

  }

  private function parseHandleCloseCurly(PHP_Token_Close_Curly $token): void {
    if (!$this->t_classEndLine->count() == 0 &&
        $this->t_classEndLine->get($this->t_classEndLine->count() - 1) == $token->getLine()) {
      $this->t_classEndLine->pop();
      $this->t_class->pop();
    } else if ($this->t_traitEndLine !== false &&
               $this->t_traitEndLine == $token->getLine()) {
      $this->t_trait = false;
      $this->t_traitEndLine = false;
    } else if ($this->t_interfaceEndLine !== false &&
               $this->t_interfaceEndLine == $token->getLine()) {
      $this->t_interface = false;
      $this->t_interfaceEndLine = false;
    }
  }

  private function parseHandleFileInclusion(
    string $includeType,
    string $includeName,
  ): void {

    $includedFiles = $this->includes->get($includeType);

    if ($includedFiles instanceof Vector) {
      error_log(
        'JEO fileInclude='.$includeType.' includedName='.$includeName,
      );
      $includedFiles->add($includeName);
    }

  }

  private function parse(): void {

    if ($this->didParse === true) {
      return;
    }

    $this->clearParseBuffers();

    $tokens = $this->tokens();

    foreach ($tokens as $token) {

      if ($token instanceof PHP_Token_Halt_Compiler) {
        return;
      }

      if ($token instanceof PHP_Token_Include) {
        $includeType = $token->getType();
        $includeName = $token->getName();
        $this->parseHandleFileInclusion($includeType, $includeName);
        continue;
      }

      if ($token instanceof PHP_Token_Require_Once) {
        $includeType = $token->getType();
        $includeName = $token->getName();
        $this->parseHandleFileInclusion($includeType, $includeName);
        continue;
      }

      if ($token instanceof PHP_Token_Require) {
        $includeType = $token->getType();
        $includeName = $token->getName();
        $this->parseHandleFileInclusion($includeType, $includeName);
        continue;
      }

      if ($token instanceof PHP_Token_Include_Once) {
        $includeType = $token->getType();
        $includeName = $token->getName();
        $this->parseHandleFileInclusion($includeType, $includeName);
        continue;
      }

      if ($token instanceof PHP_Token_Class) {
        $this->parseHandleClass($token);
        continue;
      }

      if ($token instanceof PHP_Token_Trait) {
        $this->parseHandleTrait($token);
        continue;
      }

      if ($token instanceof PHP_Token_Interface) {
        $this->parseHandleInterface($token);
        continue;
      }

      if ($token instanceof PHP_Token_Function) {
        $this->parseHandleFunction($token);
        continue;
      }

      if ($token instanceof PHP_Token_Close_Curly) {
        $this->parseHandleCloseCurly($token);
        continue;
      }

    }

    $this->clearParseBuffers();

    $this->didParse = true;

  }

  /**
   * @return array
   */
  public function getLinesOfCode(): Map<string, int> {
    return $this->linesOfCode;
  }

  /**
   */
  public function rewind(): void {
    $this->position = 0;
  }

  /**
   * @return bool
   */
  public function valid(): bool {
    return $this->tokens->containsKey($this->position);
  }

  /**
   * @return int
   */
  public function key(): int {
    return $this->position;
  }

  /**
   * @return PHP_Token
   */
  public function current(): ?TokenInterface {
    return $this->tokens->get($this->position);
  }

  /**
   */
  public function next(): void {
    $this->position++;
  }

  /**
   * @param int $offset
   *
   * @return bool
   */
  public function offsetExists(int $offset): bool {
    return $this->tokens->containsKey($offset);
  }

  /**
   * @param int $offset
   *
   * @return mixed
   *
   * @throws OutOfBoundsException
   */
  public function offsetGet(int $offset): TokenInterface {
    $token = $this->tokens->get($offset);
    if (!$token instanceof TokenInterface) {
      throw new OutOfBoundsException(
        sprintf('No token at position "%s"', $offset),
      );
    }
    return $token;
  }

  /**
   * @param int   $offset
   * @param mixed $value
   */
  public function offsetSet(int $offset, TokenInterface $value): void {
    $this->tokens[$offset] = $value;
  }

  /**
   * @param int $offset
   *
   * @throws OutOfBoundsException
   */
  public function offsetUnset(int $offset): void {

    if (!$this->offsetExists($offset)) {
      throw new OutOfBoundsException(
        sprintf('No token at position "%s"', $offset),
      );
    }

    $this->tokens->removeKey($offset);

  }

  /**
   * Seek to an absolute position.
   *
   * @param int $position
   *
   * @throws OutOfBoundsException
   */
  public function seek(int $position): void {

    $this->position = $position;

    if (!$this->valid()) {
      throw new OutOfBoundsException(
        sprintf('No token at position "%s"', $this->position),
      );
    }
  }

  /**
   * @param string $name
   * @param int    $startLine
   * @param int    $endLine
   */
  private function addFunctionToMap(
    string $name,
    int $startLine,
    int $endLine,
  ): void {
    for ($line = $startLine; $line <= $endLine; $line++) {
      $this->lineToFunctionMap->set($line, $name);
    }
  }
}
