<?hh // strict

namespace SebastianBergmann\TokenStream\Token;

use SebastianBergmann\TokenStream\Token\Stream;
use SebastianBergmann\TokenStream\TokenInterface;

// --
// The contents of this switch is autogenerated by the ./bin/generate-token-factory-code.hh
// --
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Abstract;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Ampersand;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_And_Equal;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Array;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Array_Cast;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_As;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Async;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_At;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Await;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Backtick;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Bad_Character;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Bool_Cast;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Boolean_And;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Boolean_Or;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Break;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Callable;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Caret;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Case;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Catch;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Character;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Class;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Class_C;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Class_Name_Constant;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Clone;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Close_Bracket;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Close_Curly;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Close_Square;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Close_Tag;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Coalesce;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Colon;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Comma;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Comment;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Compiler_Halt_Offset;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Concat_Equal;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Const;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Constant_Encapsed_String;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Continue;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Curly_Open;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Dec;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Declare;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Default;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Dir;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Div;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Div_Equal;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Dnumber;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Do;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Doc_Comment;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Dollar;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Dollar_Open_Curly_Braces;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Dot;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Double_Arrow;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Double_Cast;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Double_Colon;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Double_Quotes;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Echo;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Ellipsis;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Else;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Elseif;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Empty;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Encapsed_And_Whitespace;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Enddeclare;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Endfor;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Endforeach;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Endif;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Endswitch;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Endwhile;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_End_Heredoc;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Enum;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Equal;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Equals;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Eval;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Exclamation_Mark;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Exit;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Extends;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_File;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Final;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Finally;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_For;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Foreach;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Func_C;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Function;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Global;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Goto;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Gt;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Halt_Compiler;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_If;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Implements;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_In;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Inc;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Include;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Include_Once;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Inline_Html;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Instanceof;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Insteadof;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Int_Cast;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Interface;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Is_Equal;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Is_Greater_Or_Equal;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Is_Identical;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Is_Not_Equal;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Is_Not_Identical;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Is_Smaller_Or_Equal;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Isset;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Join;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Lambda_Arrow;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Lambda_Cp;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Lambda_Op;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Line;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_List;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Lnumber;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Logical_Or;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Logical_Xor;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Logical_And;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Lt;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Method_C;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Minus;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Minus_Equal;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Mod_Equal;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Mul_Equal;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Mult;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Namespace;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_New;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Ns_C;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Ns_Separator;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Nullsafe_Object_Operator;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Num_String;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Object_Cast;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Object_Operator;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Onumber;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Open_Bracket;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Open_Curly;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Open_Square;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Open_Tag;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Open_Tag_With_Echo;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Or_Equal;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Paamayim_Nekudotayim;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Percent;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Pipe;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Plus;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Plus_Equal;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Pow;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Pow_Equal;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Print;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Private;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Protected;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Public;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Question_Mark;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Require;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Require_Once;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Return;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Semicolon;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Shape;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Sl;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Sl_Equal;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Spaceship;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Sr;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Sr_Equal;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Start_Heredoc;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Static;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_String;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_String_Cast;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_String_Varname;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Super;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Switch;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Throw;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Tilde;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Trait;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Trait_C;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Try;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Tuple;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Type;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Typelist_Gt;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Typelist_Lt;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Unset;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Unset_Cast;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Use;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Use_Function;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Var;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Variable;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Where;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_While;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Whitespace;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Xhp_Attribute;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Xhp_Category;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Xhp_Category_Label;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Xhp_Children;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Xhp_Label;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Xhp_Required;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Xhp_Tag_Gt;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Xhp_Tag_Lt;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Xhp_Text;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Xor_Equal;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Yield;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Yield_From;

use Zynga\CodeBase\V1\File;

class Factory {

  private static function createToken_A(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    switch ($name) {
      case 'PHP_Token_Abstract':
        return new PHP_Token_Abstract($text, $line, $parent, $id);
      case 'PHP_Token_Ampersand':
        return new PHP_Token_Ampersand($text, $line, $parent, $id);
      case 'PHP_Token_And_Equal':
        return new PHP_Token_And_Equal($text, $line, $parent, $id);
      case 'PHP_Token_Array':
        return new PHP_Token_Array($text, $line, $parent, $id);
      case 'PHP_Token_Array_Cast':
        return new PHP_Token_Array_Cast($text, $line, $parent, $id);
      case 'PHP_Token_As':
        return new PHP_Token_As($text, $line, $parent, $id);
      case 'PHP_Token_Async':
        return new PHP_Token_Async($text, $line, $parent, $id);
      case 'PHP_Token_At':
        return new PHP_Token_At($text, $line, $parent, $id);
      case 'PHP_Token_Await':
        return new PHP_Token_Await($text, $line, $parent, $id);
    }
    return null;
  }

  private static function createToken_B(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    switch ($name) {
      case 'PHP_Token_Backtick':
        return new PHP_Token_Backtick($text, $line, $parent, $id);
      case 'PHP_Token_Bad_Character':
        return new PHP_Token_Bad_Character($text, $line, $parent, $id);
      case 'PHP_Token_Bool_Cast':
        return new PHP_Token_Bool_Cast($text, $line, $parent, $id);
      case 'PHP_Token_Boolean_And':
        return new PHP_Token_Boolean_And($text, $line, $parent, $id);
      case 'PHP_Token_Boolean_Or':
        return new PHP_Token_Boolean_Or($text, $line, $parent, $id);
      case 'PHP_Token_Break':
        return new PHP_Token_Break($text, $line, $parent, $id);
    }
    return null;
  }

  private static function createToken_C(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {

    switch ($name) {
      case 'PHP_Token_Callable':
        return new PHP_Token_Callable($text, $line, $parent, $id);
      case 'PHP_Token_Caret':
        return new PHP_Token_Caret($text, $line, $parent, $id);
      case 'PHP_Token_Case':
        return new PHP_Token_Case($text, $line, $parent, $id);
      case 'PHP_Token_Catch':
        return new PHP_Token_Catch($text, $line, $parent, $id);
      case 'PHP_Token_Character':
        return new PHP_Token_Character($text, $line, $parent, $id);
      case 'PHP_Token_Class':
        return new PHP_Token_Class($text, $line, $parent, $id);
      case 'PHP_Token_Class_C':
        return new PHP_Token_Class_C($text, $line, $parent, $id);
      case 'PHP_Token_Class_Name_Constant':
        return new PHP_Token_Class_Name_Constant($text, $line, $parent, $id);
      case 'PHP_Token_Clone':
        return new PHP_Token_Clone($text, $line, $parent, $id);
      case 'PHP_Token_Close_Bracket':
        return new PHP_Token_Close_Bracket($text, $line, $parent, $id);
      case 'PHP_Token_Close_Curly':
        return new PHP_Token_Close_Curly($text, $line, $parent, $id);
      case 'PHP_Token_Close_Square':
        return new PHP_Token_Close_Square($text, $line, $parent, $id);
      case 'PHP_Token_Close_Tag':
        return new PHP_Token_Close_Tag($text, $line, $parent, $id);
      case 'PHP_Token_Coalesce':
        return new PHP_Token_Coalesce($text, $line, $parent, $id);
      case 'PHP_Token_Colon':
        return new PHP_Token_Colon($text, $line, $parent, $id);
      case 'PHP_Token_Comma':
        return new PHP_Token_Comma($text, $line, $parent, $id);
      case 'PHP_Token_Comment':
        return new PHP_Token_Comment($text, $line, $parent, $id);
      case 'PHP_Token_Compiler_Halt_Offset':
        return new PHP_Token_Compiler_Halt_Offset($text, $line, $parent, $id);
      case 'PHP_Token_Concat_Equal':
        return new PHP_Token_Concat_Equal($text, $line, $parent, $id);
      case 'PHP_Token_Const':
        return new PHP_Token_Const($text, $line, $parent, $id);
      case 'PHP_Token_Constant_Encapsed_String':
        return
          new PHP_Token_Constant_Encapsed_String($text, $line, $parent, $id);
      case 'PHP_Token_Continue':
        return new PHP_Token_Continue($text, $line, $parent, $id);
      case 'PHP_Token_Curly_Open':
        return new PHP_Token_Curly_Open($text, $line, $parent, $id);
    }
    return null;
  }

  private static function createToken_D(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    switch ($name) {
      case 'PHP_Token_Dec':
        return new PHP_Token_Dec($text, $line, $parent, $id);
      case 'PHP_Token_Declare':
        return new PHP_Token_Declare($text, $line, $parent, $id);
      case 'PHP_Token_Default':
        return new PHP_Token_Default($text, $line, $parent, $id);
      case 'PHP_Token_Dir':
        return new PHP_Token_Dir($text, $line, $parent, $id);
      case 'PHP_Token_Div':
        return new PHP_Token_Div($text, $line, $parent, $id);
      case 'PHP_Token_Div_Equal':
        return new PHP_Token_Div_Equal($text, $line, $parent, $id);
      case 'PHP_Token_Dnumber':
        return new PHP_Token_Dnumber($text, $line, $parent, $id);
      case 'PHP_Token_Do':
        return new PHP_Token_Do($text, $line, $parent, $id);
      case 'PHP_Token_Doc_Comment':
        return new PHP_Token_Doc_Comment($text, $line, $parent, $id);
      case 'PHP_Token_Dollar':
        return new PHP_Token_Dollar($text, $line, $parent, $id);
      case 'PHP_Token_Dollar_Open_Curly_Braces':
        return
          new PHP_Token_Dollar_Open_Curly_Braces($text, $line, $parent, $id);
      case 'PHP_Token_Dot':
        return new PHP_Token_Dot($text, $line, $parent, $id);
      case 'PHP_Token_Double_Arrow':
        return new PHP_Token_Double_Arrow($text, $line, $parent, $id);
      case 'PHP_Token_Double_Cast':
        return new PHP_Token_Double_Cast($text, $line, $parent, $id);
      case 'PHP_Token_Double_Colon':
        return new PHP_Token_Double_Colon($text, $line, $parent, $id);
      case 'PHP_Token_Double_Quotes':
        return new PHP_Token_Double_Quotes($text, $line, $parent, $id);
    }
    return null;
  }

  private static function createToken_E(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    switch ($name) {
      case 'PHP_Token_Echo':
        return new PHP_Token_Echo($text, $line, $parent, $id);
      case 'PHP_Token_Ellipsis':
        return new PHP_Token_Ellipsis($text, $line, $parent, $id);
      case 'PHP_Token_Else':
        return new PHP_Token_Else($text, $line, $parent, $id);
      case 'PHP_Token_Elseif':
        return new PHP_Token_Elseif($text, $line, $parent, $id);
      case 'PHP_Token_Empty':
        return new PHP_Token_Empty($text, $line, $parent, $id);
      case 'PHP_Token_Encapsed_And_Whitespace':
        return
          new PHP_Token_Encapsed_And_Whitespace($text, $line, $parent, $id);
      case 'PHP_Token_Enddeclare':
        return new PHP_Token_Enddeclare($text, $line, $parent, $id);
      case 'PHP_Token_Endfor':
        return new PHP_Token_Endfor($text, $line, $parent, $id);
      case 'PHP_Token_Endforeach':
        return new PHP_Token_Endforeach($text, $line, $parent, $id);
      case 'PHP_Token_Endif':
        return new PHP_Token_Endif($text, $line, $parent, $id);
      case 'PHP_Token_Endswitch':
        return new PHP_Token_Endswitch($text, $line, $parent, $id);
      case 'PHP_Token_Endwhile':
        return new PHP_Token_Endwhile($text, $line, $parent, $id);
      case 'PHP_Token_End_HereDoc':
        return new PHP_Token_End_Heredoc($text, $line, $parent, $id);
      case 'PHP_Token_Enum':
        return new PHP_Token_Enum($text, $line, $parent, $id);
      case 'PHP_Token_Equal':
        return new PHP_Token_Equal($text, $line, $parent, $id);
      case 'PHP_Token_Equals':
        return new PHP_Token_Equals($text, $line, $parent, $id);
      case 'PHP_Token_Eval':
        return new PHP_Token_Eval($text, $line, $parent, $id);
      case 'PHP_Token_Exclamation_Mark':
        return new PHP_Token_Exclamation_Mark($text, $line, $parent, $id);
      case 'PHP_Token_Exit':
        return new PHP_Token_Exit($text, $line, $parent, $id);
      case 'PHP_Token_Extends':
        return new PHP_Token_Extends($text, $line, $parent, $id);
    }
    return null;
  }

  private static function createToken_F(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    switch ($name) {
      case 'PHP_Token_File':
        return new PHP_Token_File($text, $line, $parent, $id);
      case 'PHP_Token_Final':
        return new PHP_Token_Final($text, $line, $parent, $id);
      case 'PHP_Token_Finally':
        return new PHP_Token_Finally($text, $line, $parent, $id);
      case 'PHP_Token_For':
        return new PHP_Token_For($text, $line, $parent, $id);
      case 'PHP_Token_Foreach':
        return new PHP_Token_Foreach($text, $line, $parent, $id);
      case 'PHP_Token_Func_C':
        return new PHP_Token_Func_C($text, $line, $parent, $id);
      case 'PHP_Token_Function':
        return new PHP_Token_Function($text, $line, $parent, $id);
    }
    return null;
  }

  private static function createToken_G(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    switch ($name) {
      case 'PHP_Token_Global':
        return new PHP_Token_Global($text, $line, $parent, $id);
      case 'PHP_Token_Goto':
        return new PHP_Token_Goto($text, $line, $parent, $id);
      case 'PHP_Token_Gt':
        return new PHP_Token_Gt($text, $line, $parent, $id);
    }
    return null;
  }

  private static function createToken_H(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    if ($name == 'PHP_Token_Halt_Compiler') {
      return new PHP_Token_Halt_Compiler($text, $line, $parent, $id);
    }
    return null;
  }

  private static function createToken_I(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    switch ($name) {
      case 'PHP_Token_If':
        return new PHP_Token_If($text, $line, $parent, $id);
      case 'PHP_Token_Implements':
        return new PHP_Token_Implements($text, $line, $parent, $id);
      case 'PHP_Token_In':
        return new PHP_Token_In($text, $line, $parent, $id);
      case 'PHP_Token_Inc':
        return new PHP_Token_Inc($text, $line, $parent, $id);
      case 'PHP_Token_Include':
        return new PHP_Token_Include($text, $line, $parent, $id);
      case 'PHP_Token_Include_Once':
        return new PHP_Token_Include_Once($text, $line, $parent, $id);
      case 'PHP_Token_Inline_Html':
        return new PHP_Token_Inline_Html($text, $line, $parent, $id);
      case 'PHP_Token_Instanceof':
        return new PHP_Token_Instanceof($text, $line, $parent, $id);
      case 'PHP_Token_Insteadof':
        return new PHP_Token_Insteadof($text, $line, $parent, $id);
      case 'PHP_Token_Int_Cast':
        return new PHP_Token_Int_Cast($text, $line, $parent, $id);
      case 'PHP_Token_Interface':
        return new PHP_Token_Interface($text, $line, $parent, $id);
      case 'PHP_Token_Is_Equal':
        return new PHP_Token_Is_Equal($text, $line, $parent, $id);
      case 'PHP_Token_Is_Greater_Or_Equal':
        return new PHP_Token_Is_Greater_Or_Equal($text, $line, $parent, $id);
      case 'PHP_Token_Is_Identical':
        return new PHP_Token_Is_Identical($text, $line, $parent, $id);
      case 'PHP_Token_Is_Not_Equal':
        return new PHP_Token_Is_Not_Equal($text, $line, $parent, $id);
      case 'PHP_Token_Is_Not_Identical':
        return new PHP_Token_Is_Not_Identical($text, $line, $parent, $id);
      case 'PHP_Token_Is_Smaller_Or_Equal':
        return new PHP_Token_Is_Smaller_Or_Equal($text, $line, $parent, $id);
      case 'PHP_Token_Isset':
        return new PHP_Token_Isset($text, $line, $parent, $id);
    }
    return null;
  }

  private static function createToken_J(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    switch ($name) {
      case 'PHP_Token_Join':
        return new PHP_Token_Join($text, $line, $parent, $id);
    }
    return null;
  }

  private static function createToken_K(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    return null; // No K tokens atm.
  }

  private static function createToken_L(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    switch ($name) {
      case 'PHP_Token_Lambda_Arrow':
        return new PHP_Token_Lambda_Arrow($text, $line, $parent, $id);
      case 'PHP_Token_Lambda_Cp':
        return new PHP_Token_Lambda_Cp($text, $line, $parent, $id);
      case 'PHP_Token_Lambda_Op':
        return new PHP_Token_Lambda_Op($text, $line, $parent, $id);
      case 'PHP_Token_Line':
        return new PHP_Token_Line($text, $line, $parent, $id);
      case 'PHP_Token_List':
        return new PHP_Token_List($text, $line, $parent, $id);
      case 'PHP_Token_Lnumber':
        return new PHP_Token_Lnumber($text, $line, $parent, $id);
      case 'PHP_Token_Logical_Or':
        return new PHP_Token_Logical_Or($text, $line, $parent, $id);
      case 'PHP_Token_Logical_Xor':
        return new PHP_Token_Logical_Xor($text, $line, $parent, $id);
      case 'PHP_Token_Logicial_And':
        return new PHP_Token_Logical_And($text, $line, $parent, $id);
      case 'PHP_Token_Lt':
        return new PHP_Token_Lt($text, $line, $parent, $id);
    }
    return null;
  }

  private static function createToken_M(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    switch ($name) {
      case 'PHP_Token_Method_C':
        return new PHP_Token_Method_C($text, $line, $parent, $id);
      case 'PHP_Token_Minus':
        return new PHP_Token_Minus($text, $line, $parent, $id);
      case 'PHP_Token_Minus_Equal':
        return new PHP_Token_Minus_Equal($text, $line, $parent, $id);
      case 'PHP_Token_Mod_Equal':
        return new PHP_Token_Mod_Equal($text, $line, $parent, $id);
      case 'PHP_Token_Mul_Equal':
        return new PHP_Token_Mul_Equal($text, $line, $parent, $id);
      case 'PHP_Token_Mult':
        return new PHP_Token_Mult($text, $line, $parent, $id);
    }
    return null;
  }

  private static function createToken_N(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    switch ($name) {
      case 'PHP_Token_Namespace':
        return new PHP_Token_Namespace($text, $line, $parent, $id);
      case 'PHP_Token_New':
        return new PHP_Token_New($text, $line, $parent, $id);
      case 'PHP_Token_Ns_C':
        return new PHP_Token_Ns_C($text, $line, $parent, $id);
      case 'PHP_Token_Ns_Separator':
        return new PHP_Token_Ns_Separator($text, $line, $parent, $id);
      case 'PHP_Token_Nullsafe_Object_Operator':
        return
          new PHP_Token_Nullsafe_Object_Operator($text, $line, $parent, $id);
      case 'PHP_Token_Num_String':
        return new PHP_Token_Num_String($text, $line, $parent, $id);
    }
    return null;
  }

  private static function createToken_O(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    switch ($name) {
      case 'PHP_Token_Object_Cast':
        return new PHP_Token_Object_Cast($text, $line, $parent, $id);
      case 'PHP_Token_Object_Operator':
        return new PHP_Token_Object_Operator($text, $line, $parent, $id);
      case 'PHP_Token_Onumber':
        return new PHP_Token_Onumber($text, $line, $parent, $id);
      case 'PHP_Token_Open_Bracket':
        return new PHP_Token_Open_Bracket($text, $line, $parent, $id);
      case 'PHP_Token_Open_Curly':
        return new PHP_Token_Open_Curly($text, $line, $parent, $id);
      case 'PHP_Token_Open_Square':
        return new PHP_Token_Open_Square($text, $line, $parent, $id);
      case 'PHP_Token_Open_Tag':
        return new PHP_Token_Open_Tag($text, $line, $parent, $id);
      case 'PHP_Token_Open_Tag_With_Echo':
        return new PHP_Token_Open_Tag_With_Echo($text, $line, $parent, $id);
      case 'PHP_Token_Or_Equal':
        return new PHP_Token_Or_Equal($text, $line, $parent, $id);
    }
    return null;
  }

  private static function createToken_P(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    switch ($name) {
      case 'PHP_Token_Paamayim_Nekudotayim':
        return new PHP_Token_Paamayim_Nekudotayim($text, $line, $parent, $id);
      case 'PHP_Token_Percent':
        return new PHP_Token_Percent($text, $line, $parent, $id);
      case 'PHP_Token_Pipe':
        return new PHP_Token_Pipe($text, $line, $parent, $id);
      case 'PHP_Token_Plus':
        return new PHP_Token_Plus($text, $line, $parent, $id);
      case 'PHP_Token_Plus_Equal':
        return new PHP_Token_Plus_Equal($text, $line, $parent, $id);
      case 'PHP_Token_Pow':
        return new PHP_Token_Pow($text, $line, $parent, $id);
      case 'PHP_Token_Pow_Equal':
        return new PHP_Token_Pow_Equal($text, $line, $parent, $id);
      case 'PHP_Token_Print':
        return new PHP_Token_Print($text, $line, $parent, $id);
      case 'PHP_Token_Private':
        return new PHP_Token_Private($text, $line, $parent, $id);
      case 'PHP_Token_Protected':
        return new PHP_Token_Protected($text, $line, $parent, $id);
      case 'PHP_Token_Public':
        return new PHP_Token_Public($text, $line, $parent, $id);
    }
    return null;
  }

  private static function createToken_Q(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    switch ($name) {
      case 'PHP_Token_Question_Mark':
        return new PHP_Token_Question_Mark($text, $line, $parent, $id);
    }
    return null;
  }

  private static function createToken_R(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    switch ($name) {
      case 'PHP_Token_Require':
        return new PHP_Token_Require($text, $line, $parent, $id);
      case 'PHP_Token_Require_Once':
        return new PHP_Token_Require_Once($text, $line, $parent, $id);
      case 'PHP_Token_Return':
        return new PHP_Token_Return($text, $line, $parent, $id);
    }
    return null;
  }

  private static function createToken_S(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    switch ($name) {
      case 'PHP_Token_Semicolon':
        return new PHP_Token_Semicolon($text, $line, $parent, $id);
      case 'PHP_Token_Shape':
        return new PHP_Token_Shape($text, $line, $parent, $id);
      case 'PHP_Token_Sl':
        return new PHP_Token_Sl($text, $line, $parent, $id);
      case 'PHP_Token_Sl_Equal':
        return new PHP_Token_Sl_Equal($text, $line, $parent, $id);
      case 'PHP_Token_Spaceship':
        return new PHP_Token_Spaceship($text, $line, $parent, $id);
      case 'PHP_Token_Sr':
        return new PHP_Token_Sr($text, $line, $parent, $id);
      case 'PHP_Token_Sr_Equal':
        return new PHP_Token_Sr_Equal($text, $line, $parent, $id);
      case 'PHP_Token_Start_Heredoc':
        return new PHP_Token_Start_Heredoc($text, $line, $parent, $id);
      case 'PHP_Token_Static':
        return new PHP_Token_Static($text, $line, $parent, $id);
      case 'PHP_Token_String':
        return new PHP_Token_String($text, $line, $parent, $id);
      case 'PHP_Token_String_Cast':
        return new PHP_Token_String_Cast($text, $line, $parent, $id);
      case 'PHP_Token_String_Varname':
        return new PHP_Token_String_Varname($text, $line, $parent, $id);
      case 'PHP_Token_Super':
        return new PHP_Token_Super($text, $line, $parent, $id);
      case 'PHP_Token_Switch':
        return new PHP_Token_Switch($text, $line, $parent, $id);
    }
    return null;
  }

  private static function createToken_T(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    switch ($name) {
      case 'PHP_Token_Throw':
        return new PHP_Token_Throw($text, $line, $parent, $id);
      case 'PHP_Token_Tilde':
        return new PHP_Token_Tilde($text, $line, $parent, $id);
      case 'PHP_Token_Trait':
        return new PHP_Token_Trait($text, $line, $parent, $id);
      case 'PHP_Token_Trait_C':
        return new PHP_Token_Trait_C($text, $line, $parent, $id);
      case 'PHP_Token_Try':
        return new PHP_Token_Try($text, $line, $parent, $id);
      case 'PHP_Token_Tuple':
        return new PHP_Token_Tuple($text, $line, $parent, $id);
      case 'PHP_Token_Type':
        return new PHP_Token_Type($text, $line, $parent, $id);
      case 'PHP_Token_Typelist_Gt':
        return new PHP_Token_Typelist_Gt($text, $line, $parent, $id);
      case 'PHP_Token_Typelist_Lt':
        return new PHP_Token_Typelist_Lt($text, $line, $parent, $id);
    }
    return null;
  }

  private static function createToken_U(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    switch ($name) {
      case 'PHP_Token_Unset':
        return new PHP_Token_Unset($text, $line, $parent, $id);
      case 'PHP_Token_Unset_Cast':
        return new PHP_Token_Unset_Cast($text, $line, $parent, $id);
      case 'PHP_Token_Use':
        return new PHP_Token_Use($text, $line, $parent, $id);
      case 'PHP_Token_Use_Function':
        return new PHP_Token_Use_Function($text, $line, $parent, $id);
    }
    return null;
  }

  private static function createToken_V(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    switch ($name) {
      case 'PHP_Token_Var':
        return new PHP_Token_Var($text, $line, $parent, $id);
      case 'PHP_Token_Variable':
        return new PHP_Token_Variable($text, $line, $parent, $id);
    }
    return null;
  }

  private static function createToken_W(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    switch ($name) {
      case 'PHP_Token_Where':
        return new PHP_Token_Where($text, $line, $parent, $id);
      case 'PHP_Token_While':
        return new PHP_Token_While($text, $line, $parent, $id);
      case 'PHP_Token_Whitespace':
        return new PHP_Token_Whitespace($text, $line, $parent, $id);
    }
    return null;
  }

  private static function createToken_X(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    switch ($name) {
      case 'PHP_Token_Xhp_Attribute':
        return new PHP_Token_Xhp_Attribute($text, $line, $parent, $id);
      case 'PHP_Token_Xhp_Category':
        return new PHP_Token_Xhp_Category($text, $line, $parent, $id);
      case 'PHP_Token_Xhp_Category_Label':
        return new PHP_Token_Xhp_Category_Label($text, $line, $parent, $id);
      case 'PHP_Token_Xhp_Children':
        return new PHP_Token_Xhp_Children($text, $line, $parent, $id);
      case 'PHP_Token_Xhp_Label':
        return new PHP_Token_Xhp_Label($text, $line, $parent, $id);
      case 'PHP_Token_Xhp_Required':
        return new PHP_Token_Xhp_Required($text, $line, $parent, $id);
      case 'PHP_Token_Xhp_Tag_Gt':
        return new PHP_Token_Xhp_Tag_Gt($text, $line, $parent, $id);
      case 'PHP_Token_Xhp_Tag_Lt':
        return new PHP_Token_Xhp_Tag_Lt($text, $line, $parent, $id);
      case 'PHP_Token_Xhp_Text':
        return new PHP_Token_Xhp_Text($text, $line, $parent, $id);
      case 'PHP_Token_Xor_Equal':
        return new PHP_Token_Xor_Equal($text, $line, $parent, $id);
    }
    return null;
  }

  private static function createToken_Y(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    switch ($name) {
      case 'PHP_Token_Yield':
        return new PHP_Token_Yield($text, $line, $parent, $id);
    }
    return null;
  }

  private static function createToken_Z(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    return null; // no Z based tokens atm.
  }

  public static function createToken(
    string $name,
    string $text,
    int $line,
    File $parent,
    int $id,
  ): ?TokenInterface {
    $token = null;

    $firstChar = substr($name, 10, 1);

    if ($firstChar == 'A') {
      $token = self::createToken_A($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'B') {
      $token = self::createToken_B($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'C') {
      $token = self::createToken_C($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'D') {
      $token = self::createToken_D($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'E') {
      $token = self::createToken_E($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'F') {
      $token = self::createToken_F($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'G') {
      $token = self::createToken_G($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'H') {
      $token = self::createToken_H($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'I') {
      $token = self::createToken_I($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'J') {
      $token = self::createToken_J($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'K') {
      $token = self::createToken_K($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'L') {
      $token = self::createToken_L($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'M') {
      $token = self::createToken_M($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'N') {
      $token = self::createToken_N($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'O') {
      $token = self::createToken_O($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'P') {
      $token = self::createToken_P($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'Q') {
      $token = self::createToken_Q($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'R') {
      $token = self::createToken_R($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'S') {
      $token = self::createToken_S($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'T') {
      $token = self::createToken_T($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'U') {
      $token = self::createToken_U($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'V') {
      $token = self::createToken_V($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'W') {
      $token = self::createToken_W($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'X') {
      $token = self::createToken_X($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'Y') {
      $token = self::createToken_Y($name, $text, $line, $parent, $id);
    } else if ($firstChar == 'Z') {
      $token = self::createToken_Z($name, $text, $line, $parent, $id);
    }

    return $token;
  }

}
