<?hh // strict

namespace SebastianBergmann\TokenStream\Token\Stream;

use SebastianBergmann\TokenStream\Token\Stream;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Class;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Close_Curly;
// use SebastianBergmann\TokenStream\Tokens\PHP_Token_Comment;
// use SebastianBergmann\TokenStream\Tokens\PHP_Token_Doc_Comment;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Function;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Include;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Include_Once;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Halt_Compiler;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Interface;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Require;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Require_Once;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Trait;

use Zynga\CodeBase\V1\File;
use Zynga\CodeBase\V1\Code\Code_Class;
use Zynga\CodeBase\V1\Code\Code_Inclusion;
use Zynga\CodeBase\V1\Code\Code_Interface;
use Zynga\CodeBase\V1\Code\Code_Method;

class Parser {

  // jeo: made temporary variables to avoid parameter passing throughout this
  //   code as it's refactored.
  private Vector<string> $t_class;
  private Vector<int> $t_classEndLine;
  private mixed $t_trait;
  private mixed $t_traitEndLine;
  private mixed $t_interface;
  private mixed $t_interfaceEndLine;

  public function __construct() {
    $this->t_class = Vector {};
    $this->t_classEndLine = Vector {};
    $this->t_trait = false;
    $this->t_traitEndLine = false;
    $this->t_interface = false;
    $this->t_interfaceEndLine = false;
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

  public function parse(File $codeFile, Stream $stream): void {

    $this->clearParseBuffers();

    $tokens = $stream->tokens();

    foreach ($tokens as $token) {

      if ($token instanceof PHP_Token_Halt_Compiler) {
        $this->clearParseBuffers();
        return;
      }

      if ($token instanceof PHP_Token_Include) {
        $includeType = $token->getType();
        $includeName = $token->getName();
        $this->handleFileInclusion($codeFile, $includeType, $includeName);
        continue;
      }

      if ($token instanceof PHP_Token_Require_Once) {
        $includeType = $token->getType();
        $includeName = $token->getName();
        $this->handleFileInclusion($codeFile, $includeType, $includeName);
        continue;
      }

      if ($token instanceof PHP_Token_Require) {
        $includeType = $token->getType();
        $includeName = $token->getName();
        $this->handleFileInclusion($codeFile, $includeType, $includeName);
        continue;
      }

      if ($token instanceof PHP_Token_Include_Once) {
        $includeType = $token->getType();
        $includeName = $token->getName();
        $this->handleFileInclusion($codeFile, $includeType, $includeName);
        continue;
      }

      if ($token instanceof PHP_Token_Class) {
        $this->handleClass($codeFile, $token);
        continue;
      }

      if ($token instanceof PHP_Token_Trait) {
        $this->handleTrait($codeFile, $token);
        continue;
      }

      if ($token instanceof PHP_Token_Interface) {
        $this->handleInterface($codeFile, $token);
        continue;
      }

      if ($token instanceof PHP_Token_Function) {
        $this->handleFunction($codeFile, $token);
        continue;
      }

      if ($token instanceof PHP_Token_Close_Curly) {
        $this->handleCloseCurly($token);
        continue;
      }

    }

    $this->clearParseBuffers();

  }

  private function handleFileInclusion(
    File $codeFile,
    string $includeType,
    string $includeName,
  ): void {

    $inclusion = null;

    if ($includeType == 'require') {
      $inclusion = $codeFile->inclusions()->requires();
    } else if ($includeType == 'require_once') {
      $inclusion = $codeFile->inclusions()->require_onces();
    } else if ($includeType == 'include') {
      $inclusion = $codeFile->inclusions()->includes();
    } else if ($includeType == 'include_once') {
      $inclusion = $codeFile->inclusions()->include_onces();
    }

    if ($inclusion instanceof Code_Inclusion) {
      $inclusion->add($includeName);
    }

  }

  private function handleCloseCurly(PHP_Token_Close_Curly $token): void {
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

  private function handleFunction(
    File $codeFile,
    PHP_Token_Function $token,
  ): void {

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
    $tmp->file = $codeFile->getFile();

    if ($this->t_class->count() == 0 &&
        $this->t_trait === false &&
        $this->t_interface === false) {

      $codeFile->functions()->add($name, $tmp);

      // @TODO: not really sure why we ever had a function -> start/end map
      //$this->addFunctionToMap($name, $tmp->startLine, $tmp->endLine);

    } else if ($this->t_class->count() != 0 &&
               strval($this->t_class->get($this->t_class->count() - 1)) != 'anonymous class') {

      $a_className = $this->t_class->get($this->t_class->count() - 1);

      $a_class = null;
      if (is_string($a_className)) {
        $a_class = $codeFile->classes()->get($a_className);
      }

      if ($a_class instanceof Code_Class) {
        $a_class->methods->set($name, $tmp);

        // @TODO: not really sure why we ever had a function ->start/end map
        // $this->addFunctionToMap(
        //   $a_className.'::'.$name,
        //   $tmp->startLine,
        //   $tmp->endLine,
        // );

      }

    } else if ($this->t_trait !== false) {

      $a_traitname = strval($this->t_trait);
      $a_trait = null;

      $a_trait = $codeFile->traits()->get($a_traitname);

      if ($a_trait instanceof Code_Class) {

        $a_trait->methods->set($name, $tmp);

        // @TODO: still don't know why we have a static map
        // $this->addFunctionToMap(
        //   $a_traitname.'::'.$name,
        //   $tmp->startLine,
        //   $tmp->endLine,
        // );
      }

    } else {

      $a_interfacename = strval($this->t_interface);
      $a_interface = $codeFile->interfaces()->get($a_interfacename);
      if ($a_interface instanceof Code_Interface) {
        $a_interface->methods->set($name, $tmp);
      }
    }

  }

  private function handleTrait(File $codeFile, PHP_Token_Trait $token): void {

    $tmp = new Code_Class();
    $tmp->methods->clear();
    $tmp->parent = $token->getParent();
    $tmp->interfaces = $token->getInterfaces();
    $tmp->keywords = $token->getKeywords();
    $tmp->docblock = strval($token->getDocblock());
    $tmp->startLine = $token->getLine();
    $tmp->endLine = $token->getEndLine();
    $tmp->package = $token->getPackage();
    $tmp->file = $codeFile->getFile();

    $this->t_trait = $token->getName();
    $this->t_traitEndLine = $token->getEndLine();

    $codeFile->traits()->add($token->getName(), $tmp);

  }

  private function handleClass(File $codeFile, PHP_Token_Class $token): void {

    $tmp = new Code_Class();
    $tmp->methods->clear();
    $tmp->parent = $token->getParent();
    $tmp->interfaces = $token->getInterfaces();
    $tmp->keywords = $token->getKeywords();
    $tmp->docblock = strval($token->getDocblock());
    $tmp->startLine = $token->getLine();
    $tmp->endLine = $token->getEndLine();
    $tmp->package = $token->getPackage();
    $tmp->file = $codeFile->getFile();

    $this->t_class->add($token->getName());

    $this->t_classEndLine->add($token->getEndLine());

    $previousClass = $this->t_class->get($this->t_class->count() - 1);

    if (is_string($previousClass) && $previousClass != 'anonymous class') {
      $codeFile->classes()->add($previousClass, $tmp);
    }

  }

  private function handleInterface(
    File $codeFile,
    PHP_Token_Interface $token,
  ): void {

    $interface = $token->getName();
    $interfaceEndLine = $token->getEndLine();

    $tmp = new Code_Interface();
    $tmp->methods->clear();
    $tmp->parent = $token->getParent();
    $tmp->keywords = $token->getKeywords();
    $tmp->docblock = strval($token->getDocblock());
    $tmp->startLine = $token->getLine();
    $tmp->endLine = $interfaceEndLine;
    $tmp->package = $token->getPackage();
    $tmp->file = $codeFile->getFile();

    $codeFile->interfaces()->add($interface, $tmp);
    $this->t_interface = $interface;

  }

}
