<?hh // strict

namespace Zynga\CodeBase\V1;

use SebastianBergmann\CodeCoverage\Driver;
use SebastianBergmann\CodeCoverage\Driver\HHVM\ExecBlock;
use SebastianBergmann\CodeCoverage\Driver\HHVM\Logging as HHVM_Logging;
use SebastianBergmann\TokenStream\Token\Stream;
use SebastianBergmann\TokenStream\Token\Stream\Scanner as StreamScanner;
use SebastianBergmann\TokenStream\Token\Stream\Parser as StreamParser;
use SebastianBergmann\TokenStream\Token\Stream\CachingFactory;
use SebastianBergmann\TokenStream\Token\Types;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Abstract;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Catch;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Finally;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Class;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Close_Bracket;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Close_Curly;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Comment;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Double_Colon;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Echo;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Else;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Elseif;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_If;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Invariant;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Foreach;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Function;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Nullsafe_Object_Operator;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Object_Operator;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Open_Bracket;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Private;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Protected;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Public;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Return;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Semicolon;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_String;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Switch;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Throw;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Try;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_While;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Whitespace;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Variable;

use SebastianBergmann\TokenStream\TokenInterface;

use Zynga\CodeBase\V1\File\Classes;
use Zynga\CodeBase\V1\File\Functions;
use Zynga\CodeBase\V1\File\Inclusions;
use Zynga\CodeBase\V1\File\Interfaces;
use Zynga\CodeBase\V1\File\LineExecutionState;
use Zynga\CodeBase\V1\File\RawTokens;
use Zynga\CodeBase\V1\File\Source;
use Zynga\CodeBase\V1\File\Stats;
use Zynga\CodeBase\V1\File\Traits;

class File {
  private bool $_didInit;
  private string $_file;
  private Map<int, Map<string, bool>> $_lineToTests;
  private int $_startLine;
  private int $_endLine;

  private ?Classes $_classes;
  private ?Functions $_functions;
  private ?Inclusions $_inclusions;
  private ?Interfaces $_interfaces;
  private ?LineExecutionState $_lineExecutionState;
  private ?RawTokens $_rawTokens;
  private ?Source $_source;
  private ?Stats $_stats;
  private ?Stream $_stream;
  private ?Traits $_traits;

  public function __construct(string $file) {
    $this->_didInit = false;
    $this->_file = $file;
    $this->_lineToTests = Map {};
    $this->_startLine = -1;
    $this->_endLine = -1;

    $this->_classes = null;
    $this->_functions = null;
    $this->_inclusions = null;
    $this->_interfaces = null;
    $this->_lineExecutionState = null;
    $this->_rawTokens = null;
    $this->_source = null;
    $this->_stats = null;
    $this->_stream = null;
    $this->_traits = null;

  }

  public function getFile(): string {
    return $this->_file;
  }

  public function source(): Source {
    if ($this->_source instanceof Source) {
      return $this->_source;
    }

    $this->_source = new Source($this);
    return $this->_source;

  }

  public function rawTokens(): RawTokens {
    if ($this->_rawTokens instanceof RawTokens) {
      return $this->_rawTokens;
    }

    $this->_rawTokens = new RawTokens($this);
    return $this->_rawTokens;

  }

  public function stats(): Stats {

    if ($this->_stats instanceof Stats) {
      return $this->_stats;
    }

    $this->_stats = new Stats($this);
    return $this->_stats;

  }

  public function lineExecutionState(): LineExecutionState {

    // We can only call this -after- stream has already populated.
    if ($this->_lineExecutionState instanceof LineExecutionState) {
      return $this->_lineExecutionState;
    }

    $this->_lineExecutionState = new LineExecutionState($this);

    return $this->_lineExecutionState;

  }

  public function functions(): Functions {

    if ($this->_functions instanceof Functions) {
      return $this->_functions;
    }

    $this->_functions = new Functions($this);

    return $this->_functions;

  }

  public function classes(): Classes {

    if ($this->_classes instanceof Classes) {
      return $this->_classes;
    }

    $this->_classes = new Classes($this);

    return $this->_classes;

  }

  public function traits(): Traits {

    if ($this->_traits instanceof Traits) {
      return $this->_traits;
    }

    $this->_traits = new Traits($this);

    return $this->_traits;

  }

  public function interfaces(): Interfaces {

    if ($this->_interfaces instanceof Interfaces) {
      return $this->_interfaces;
    }

    $this->_interfaces = new Interfaces($this);

    return $this->_interfaces;

  }

  public function inclusions(): Inclusions {

    if ($this->_inclusions instanceof Inclusions) {
      return $this->_inclusions;
    }

    $this->_inclusions = new Inclusions($this);

    return $this->_inclusions;

  }

  public function getLinesToTests(): Map<int, Vector<string>> {
    $data = Map {};

    // unroll the map, into a vector for ease of use.
    foreach ($this->_lineToTests as $lineNo => $testsMap) {
      $data->set($lineNo, $testsMap->keys());
    }

    return $data;
  }

  public function setLineToTest(int $lineNo, string $testId): void {

    $data = $this->_lineToTests->get($lineNo);

    if (!$data instanceof Map) {
      $data = Map {};
    }

    $data->set($testId, true);

    $this->_lineToTests->set($lineNo, $data);

  }

  public function lineToTestToArrayFormat(): array<int, array<string>> {
    $data = array();

    foreach ($this->_lineToTests as $lineNo => $testsMap) {
      $data[$lineNo] = array();
      foreach ($testsMap as $testId => $tested) {
        $data[$lineNo][] = $testId;
      }
    }

    return $data;
  }

  public function stream(): Stream {

    if ($this->_stream instanceof Stream) {
      return $this->_stream;
    }

    $stream = new Stream($this);

    $scanner = new StreamScanner();
    $scanner->scan($this, $stream);
    $this->_stream = $stream;

    $parser = new StreamParser();
    $parser->parse($this, $stream);

    $this->_stream = $stream;
    return $this->_stream;

  }

  private function isTokenExecutable(
    TokenInterface $token,
    int $currentLine,
  ): (bool, int, string) {

    if ($token instanceof PHP_Token_Return) {
      $skipAmount = $token->getEndOfDefinitionLineNo() - $currentLine;
      return tuple(true, $skipAmount, 'return');
    }

    if ($token instanceof PHP_Token_Variable) {
      $skipAmount = 0;
      return tuple(true, $skipAmount, 'variable');
    }

    if ($token instanceof PHP_Token_Throw) {
      $skipAmount = $token->getEndOfDefinitionLineNo() - $currentLine;
      return tuple(true, $skipAmount, 'throw');
    }

    $skipAmount = 0;
    return tuple(false, $skipAmount, '');

  }

  private function isTokenComment(TokenInterface $token): bool {
    return false;
  }

  private function isTokenOperator(TokenInterface $token): bool {
    if ( $token->getTokenType() == Types::T_OPERATOR && ! $token instanceof PHP_Token_Object_Operator ) {
      return true;
    }
    return false;
  }

  private function handleVariableExecutionStatus(
      Vector<TokenInterface> $lineStack,
      int $currentLine, 
      int $offset, 
      TokenInterface $token, 
      bool $isPublic, 
      bool $isPrivate, 
      bool $isProtected, 
      bool $isExecutable, 
      int $skipAmount, 
      string $reason
      ): (bool, int, string) {

    // public|private|protected $baz != executable code.
    // this is a class variable of some sort due to it's accessability level.
    if ( $isPublic === true || $isPrivate === true || $isProtected === true) {
      return tuple(false, 0, '');
    }

    // $foo (=|+=|...) variable operator support
    // does this line contain an operator?
    $tokenCount = $lineStack->count();

    for ( $i = $offset + 1; $i < $tokenCount; $i++ ) {
      $futureToken = $lineStack->get($i);
      if ( ! $futureToken instanceof TokenInterface ) {
        break;
      }
      if ( $futureToken instanceof PHP_Token_Whitespace ) {
        continue;
      }
      if ( $this->isTokenOperator($futureToken) ) {
        $skipAmount = $futureToken->getEndOfDefinitionLineNo() - $currentLine;
        return tuple(true, $skipAmount, 'variable-with-operator');
      }
    }

    // --
    // Need to support $foo->baz(\w*);
    //
    // In order to do such we need to pull the raw stream in.
    //
    // --
    $allTokens = $this->stream()->tokens();
    $allTokenCount = $allTokens->count();

    $objectOperator = null;
    $block = 0;

    for ( $i = $token->getId(); $i < $allTokenCount; $i++ ) {

      $futureToken = $allTokens->get($i);
            
      if ( ! $futureToken instanceof TokenInterface ) {
        break;
      }

      if ( $futureToken instanceof PHP_Token_Whitespace ) {
        continue;
      } 

      if ( $futureToken instanceof PHP_Token_Nullsafe_Object_Operator ) {
        continue;
      }
      
      if ( $futureToken instanceof PHP_Token_Object_Operator ) {
        $objectOperator = $futureToken;
        break;
      }

    }

    if ( $objectOperator instanceof PHP_Token_Object_Operator ) {
      $reason = 'variable-object-function-call';
      $skipAmount = $objectOperator->getEndOfDefinitionLineNo() - $currentLine;
      $this->markExecutableForDefintionToClose($objectOperator, $reason, true);
      return tuple(false, $skipAmount, $reason);
    }

    // we found a executable token, stop working so hard
    return tuple($isExecutable, $skipAmount, $reason);

  }

  private function markExecutableForDefintionToClose(
      TokenInterface $token, 
      string $reason, 
      bool $markEndOfBlock
      ): void {

    $currentLine = $token->getLine();
    $endOfDef    = $token->getEndOfDefinitionLineNo();
    $endOfBlock  = $token->getEndLine();
    $lineToTokens = $this->stream()->getLineToTokensForLine();

    /*
    JEO:
    if ( preg_match('/UserIterator/', $this->_file) ) {
    echo "currentLine=$currentLine endOfDef=$endOfDef endOfBlock=$endOfBlock\n";
    }
    */

    // --
    // The whole definition gets marked as a 'executable' component, with the 'innards' being the trigger 
    // for successful execution.
    // --
    // 1) Mark all of the defition as not executed.
    for ( $defLineNo = $currentLine; $defLineNo <= $endOfDef; $defLineNo++ ) {

      $lineTokens = $lineToTokens->get($defLineNo);

      if ( ! $lineTokens instanceof Vector ) {
        continue;
      }

      if ( $lineTokens->count() == 0 ) {
        continue;
      }

      // Check for blank or comment only lines before saying it should be executed.
      $nonWhitespaceTokens = false;
      foreach ( $lineTokens as $lineToken ) {
        if ( $lineToken instanceof PHP_Token_Whitespace ) {
          continue;
        } else if ( $lineToken instanceof PHP_Token_Comment ) {
          continue;
        } else {
          $nonWhitespaceTokens = true;
          break;
        }
      }

      if ( $nonWhitespaceTokens === true ) {
        $this->lineExecutionState()->set($defLineNo, Driver::LINE_NOT_EXECUTED);
      }

    }

    // 2) This is the last line in the block that needs to be marked up as non-executed.
    if ( $markEndOfBlock === true && $currentLine != $endOfBlock ) {
      $this->lineExecutionState()->set($endOfBlock, Driver::LINE_NOT_EXECUTED);
    }

    // 3) Wire up a block line => token definition trigger && end of token trigger.
    for ( $blockLineNo = $endOfDef; $blockLineNo <= $endOfBlock; $blockLineNo++ ) {

      // handle the definition component for the token
      $this->lineExecutionState()->addFiniteExecutableRange(
        $reason . '-BLOCK',
        $blockLineNo,
        $currentLine,
        $endOfDef,
      );

      // handle the end of token
      if ( $markEndOfBlock === true && $currentLine != $endOfBlock ) {
        $this->lineExecutionState()->addFiniteExecutableRange(
          $reason . '-EOB',
          $blockLineNo,
          $endOfBlock,
          $endOfBlock
        );
      }

    }

  }

  private function determineLineExecutableForeachLine(
    Vector<TokenInterface> $lineStack,
    int $currentLine,
  ): (bool, int, string) {

    $offset = 0;
    $isPublic = false;
    $isPrivate = false;
    $isProtected = false;

    $isExecutable = false;
    $skipAmount = 0;
    $reason = 'none-given';

    foreach ($lineStack as $token) {

      if ( $token instanceof PHP_Token_Comment ) {
        // Everything to the right of this isn't executable.
        if ( $reason == 'none-given' ) {
          $reason = 'only-comments';
        }
        break;
      }

      if ($token instanceof PHP_Token_Class) {
        $isExecutable = false;
        $skipAmount = $token->getEndOfDefinitionLineNo() - $currentLine;
        $reason = 'class';
        return tuple($isExecutable, $skipAmount, $reason);
      }

      if ( $token instanceof PHP_Token_Abstract) {
        $isExecutable = false;
        $skipAmount = $token->getEndOfDefinitionLineNo() - $currentLine;
        $reason = 'abstract';
        return tuple($isExecutable, $skipAmount, $reason);
      }

      if ($token instanceof PHP_Token_Function) {

        $this->markExecutableForDefintionToClose($token, 'function', true);

        $isExecutable = false; // JEO: We overwrite the isExecutable via markExecutable, need to remove
        $skipAmount = $token->getEndOfDefinitionLineNo() - $currentLine;
        $reason = 'function';

        $this->debug(Map {
            'action' => 'function', 
            'function' => $token->getName(),
            'skipAmount' => $skipAmount,
            'currentLine' => $currentLine
            });

        return tuple($isExecutable, $skipAmount, $reason);
      }

      if ($token instanceof PHP_Token_If || $token instanceof PHP_Token_Else || $token instanceof PHP_Token_Elseif) {
        
        $isExecutable = false; // JEO: We overwrite the isExecutable via markExecutable, need to remove
        
        $endOfDef = $token->getEndOfDefinitionLineNo();
        $endLineNo = $token->getEndLine();
        $skipAmount = $endOfDef - $currentLine;

        // Detecif if this has a else or elseif block, as we have to lower the skip amount by 1 to allow 
        //  else | elseif to handle it's business
        $lineToTokens = $this->stream()->getLineToTokensForLine();
        $endOfDefTokens = $lineToTokens->get($endLineNo);

        $hasContinuation = false;
        if ( $endOfDefTokens instanceof Vector && $endOfDefTokens->count() > 0 ) {
          foreach ( $endOfDefTokens as $endOfDefToken ) {
            if ( $endOfDefToken instanceof PHP_Token_Else || $endOfDefToken instanceof PHP_Token_Elseif ) {

    /* 
    JEO: 
    if ( preg_match('/UserIterator/', $this->_file) ) {
              echo "file=" . $this->_file . " lineNo=" . $currentLine . " has-cont\n";
    }
    */

              $hasContinuation = true;
              break;
            }
          }
          if ( $hasContinuation === false && preg_match('/UserIterator/', $this->_file) ) {
              $tokenNames = '';
              foreach ( $endOfDefTokens as $endOfDefToken ) {
                if ( $tokenNames != '' ) { $tokenNames .= ', '; }
                $tokenNames .= $endOfDefToken->getShortTokenName();
              }
              /*
              JEO:
              echo "file=" . $this->_file . " lineNo=" . $currentLine . "\n";
              echo "no-cont: $tokenNames\n";
              */
          }
        }
        
        $reason = $token->getShortTokenName();

        if ( $hasContinuation === true ) {
          $this->markExecutableForDefintionToClose($token, $reason, false);
        } else {
          $this->markExecutableForDefintionToClose($token, $reason, true);
        }

        return tuple($isExecutable, $skipAmount, $reason);

      }
      
      if ($token instanceof PHP_Token_Switch) {
        $isExecutable = false;
        $skipAmount = $token->getEndOfDefinitionLineNo() - $currentLine;
        $reason = 'switch';
        return tuple($isExecutable, $skipAmount, $reason);
      }

      if ($token instanceof PHP_Token_Try) {

        $this->markExecutableForDefintionToClose($token, 'try', false);

        $isExecutable = false; // JEO: We overwrite the isExecutable via markExecutable, need to remove

        $endOfDef = $token->getEndOfDefinitionLineNo();
        $skipAmount = $endOfDef - $currentLine;

        $reason = 'try';
        return tuple($isExecutable, $skipAmount, $reason);

      }

      if ($token instanceof PHP_Token_Catch) {
        
        $isExecutable = false; // JEO: We overwrite the isExecutable via markExecutable, need to remove
        $reason = $token->getShortTokenName();
        $endOfDef = $token->getEndOfDefinitionLineNo();
        $skipAmount = $endOfDef - $currentLine;
   
        // Detect if this has a finally block, as we have to lower the skip amount by 1 to allow 
        //  finally to handle it's business
        $lineToTokens = $this->stream()->getLineToTokensForLine();
        $endOfDefTokens = $lineToTokens->get($endOfDef);

        $hasFinallyBlock = false;
        if ( $endOfDefTokens instanceof Vector && $endOfDefTokens->count() > 1 ) {
          foreach ( $endOfDefTokens as $endOfDefToken ) {
            if ( $endOfDefToken instanceof PHP_Token_Finally ) {
              $hasFinallyBlock = true;
            }
          }
        }
       
        if ( $hasFinallyBlock === true ) {
          $this->markExecutableForDefintionToClose($token, $reason, false);
        } else {
          $this->markExecutableForDefintionToClose($token, $reason, true);
        }

        $reason = 'catch';
        return tuple($isExecutable, $skipAmount, $reason);

      }
      
      if ($token instanceof PHP_Token_Finally) {
        $this->markExecutableForDefintionToClose($token, 'finally', true);
        $isExecutable = false; // JEO: We overwrite the isExecutable via markExecutable, need to remove
        $skipAmount = $token->getEndOfDefinitionLineNo() - $currentLine;
        $reason = 'finally';
        return tuple($isExecutable, $skipAmount, $reason);
      }
      
      if ( $token instanceof PHP_Token_Echo || 
          $token instanceof PHP_Token_Foreach || 
          $token instanceof PHP_Token_While) {
        $reason = $token->getShortTokenName();
        $this->markExecutableForDefintionToClose($token, $reason, true);
        $isExecutable = false; // JEO: We overwrite the isExecutable via markExecutable, need to remove
        $skipAmount = $token->getEndOfDefinitionLineNo() - $currentLine;
        return tuple($isExecutable, $skipAmount, $reason);
      }

      
      if ($token instanceof PHP_Token_Invariant) {
        $isExecutable = true;
        $skipAmount = $token->getEndOfDefinitionLineNo() - $currentLine;
        $reason = 'invariant';
        return tuple($isExecutable, $skipAmount, $reason);
      }

      // Handle static function calls and their blocks they create.
      if ( $token instanceof PHP_Token_Double_Colon ) {
        $t_functionName = $lineStack->get($offset + 1);
        $t_bracketToken = $lineStack->get($offset + 2);
        if ( $t_functionName instanceof PHP_Token_String && 
             $t_bracketToken instanceof PHP_Token_Open_Bracket ) {
          $reason = $token->getShortTokenName();
          $this->markExecutableForDefintionToClose($token, $reason, true);
          $isExecutable = false; // JEO: We overwrite the isExecutable via markExecutable, need to remove
          $skipAmount = $token->getEndOfDefinitionLineNo() - $currentLine;
          return tuple($isExecutable, $skipAmount, $reason);
        }
      }

      if ( $token instanceof PHP_Token_Public ) {
        $isPublic = true;
      }

      if ( $token instanceof PHP_Token_Private ) {
        $isPrivate = true;
      }

      if ( $token instanceof PHP_Token_Protected ) {
        $isProtected = true;
      }

      list($isExecutable, $skipAmount, $reason) =
        $this->isTokenExecutable($token, $currentLine);

      if ($isExecutable === true) {

        // Executable variable tokens are handled specially and in their  own handler.
        if ( $token instanceof PHP_Token_Variable ) {
          
          return $this->handleVariableExecutionStatus(
              $lineStack, $currentLine, $offset, $token, $isPublic, $isPrivate, $isProtected,
              $isExecutable, $skipAmount, $reason
            );

        }

        // we found a executable token, stop working so hard
        return tuple($isExecutable, $skipAmount, $reason);

      }

      if ($this->isTokenComment($token) === true) {
        // everything to the right of this token is useless
        break;
      }

      $offset++;

    }

    return tuple($isExecutable, 0, '');

  }

  private function determineLineExecutable(
    Vector<TokenInterface> $lineStack,
    int $currentLine,
  ): (int, string) {

    $isExecutable = false;
    $skipAmount = 0;
    $reason = '';

    if ($lineStack->count() == 0) {
      // empty stack therefor noop
    // -- JEO: probably dead with the imporoved block parsing.
    // } else if ($lineStack->count() == 1) {
    //  // if it's a trailing } from either a control structure it's a non-op
    //  $token = $lineStack->get(0);
    //  if ($token instanceof PHP_Token_Close_Curly) {
    //    $isExecutable = false;
    //  }
    } else {
      // Loop across the line looking for more complex patterns.
      list($isExecutable, $skipAmount, $reason) =
        $this->determineLineExecutableForeachLine($lineStack, $currentLine);
    }

    // mark the line up with the state.
    if ($isExecutable === true) {

      $this->debug(Map {
         'action' => 'isExecutable',
         'currentLine' => $currentLine,
         'reason' => $reason,
         'skipAmount' => $skipAmount
         });

      if ($skipAmount > 0) {
        for ($s = 0; $s < $skipAmount; $s++) {
          $this->lineExecutionState()
            ->set(($currentLine + $s), Driver::LINE_NOT_EXECUTED);
        }
        $this->lineExecutionState()->addExecutableRange(
          $reason,
          $currentLine,
          $currentLine + $skipAmount,
        );
      } else {
        $this->lineExecutionState()
          ->set($currentLine, Driver::LINE_NOT_EXECUTED);
      }
    } else {
      $this->lineExecutionState()
        ->set($currentLine, Driver::LINE_NOT_EXECUTABLE);
    }

    return tuple($skipAmount, $reason);

  }

  public function debug(Map <string, mixed> $params): void {

    return;
    /*
    //if ( ! preg_match('/Legacy\/Features\/Admin\/BoostConfig\/V1\/BoostConfigManager.hh/', $this->_file ) ) {
    //  return;
    // }
    
    if ( ! preg_match('/UserIterator/', $this->_file ) ) {
      return;
    }

    $text = '';
    $text .= 'file=' . $this->_file;

    foreach ( $params as $key => $value ) {
      if ( $text != '' ) {
        $text .= ' ';
      }
      $text .= $key . '=';
      $text .= json_encode($value);
    }

    // echo "$text\n";
    // error_log($text);
    */

  }

  public function init(): void {

    if ($this->_didInit === true) {
      return;
    }

    $this->debug(Map{'action' => 'init-start'});
    
    // load the source for this file.
    $this->source()->load();

    // get the raw tokens loaded.
    $this->rawTokens()->load();

    $lineCount = $this->stream()->getLineCount();
    $lineToTokens = $this->stream()->getLineToTokensForLine();

    $skipAmount = 0;
    $this->debug(Map{'action' => 'line-loop-start', 'lineCount' => $lineCount});
    
    $reason = '';

    for ( $lineNo = 1; $lineNo < $lineCount; $lineNo++ ) {

      $lineStack = $lineToTokens->get($lineNo);

      if ( ! $lineStack instanceof Vector ) {
        $this->debug(Map{'action' => 'line-loop-no-tokens', 'lineNo' => $lineNo});
        $lineStack = Vector {};
      } else {
        $this->debug(Map{'action' => 'line-loop-has-tokens', 'lineNo' => $lineNo, 'tokenCount' => $lineStack->count()});
      }

      // we update the endLine every pass on this loop.
      $this->_endLine = $lineNo;

      if ($skipAmount > 0) {
        $this->debug(Map{'action' => 'skipping','lineNo' => $lineNo, 'skipAmount' => $skipAmount, 'reason' => $reason});
        $skipAmount--;
      } else {
        list($skipAmount, $reason) = $this->determineLineExecutable($lineStack, $lineNo);
      }

    }

    $this->debug(Map{'action' => 'init-complete'});
    $this->_didInit = true;

  }

  public function getStartLine(): int {
    return $this->_startLine;
  }

  public function getEndLine(): int {
    return $this->_endLine;
  }

}
