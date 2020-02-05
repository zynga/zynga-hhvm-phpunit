<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\Token;
use SebastianBergmann\TokenStream\Token\Types;
use SebastianBergmann\TokenStream\TokenInterface;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Whitespace;
use SebastianBergmann\TokenStream\Tokens\PHP_Object_Operator;
use Zynga\CodeBase\V1\File;

class PHP_Token_Variable extends Token {
  private bool $didIsVariableWithOperator = false;
  private int $variableOperatorId = -1;
  private int $variableObjectOperatorId = -1;

  public function getTokenType(): string {
    return Types::T_VARIABLE;
  }

  public function getShortTokenName(): string {
    return 'Variable';
  }

  public function getEndTokenId(): int {
    return $this->getEndOfDefinitionTokenId();
  }

  public function getEndOfDefinitionTokenId(): int {

    // --
    // We need to resolve down to the end of the whole = | object operator.
    // --

    // $foo (=|+=|...) variable operator support
    // does this line contain an operator?
    $id = $this->getVariableWithOperatorTokenId();

    if ( $id == -1 ) {
      // support for $objectFoo->...
      $id = $this->getVariableWithObjectOperatorTokenId();
    }

    if ( $id !== -1 ) {

      $tokens = $this->tokenStream()->tokens();

      $token = $tokens->get($id - 1);

      if ( $token instanceof TokenInterface ) {

        return $token->getEndTokenId();

      }

    }

    // by default we are our own ending.
    return -1;

  }

  public function hasVariableWithOperator(): bool {

    $tokenId = $this->getVariableWithOperatorTokenId();

    if ($tokenId > -1) {
      return true;
    }

    return false;

  }

  public function getVariableWithOperatorTokenId(): int {

    $this->doFindVariableOperators();

    return $this->variableOperatorId;

  }

  public function hasVariableWithObjectOperator(): bool {

    $tokenId = $this->getVariableWithObjectOperatorTokenId();

    if ($tokenId > -1) {
      return true;
    }

    return false;

  }

  public function getVariableWithObjectOperatorTokenId(): int {

    $this->doFindVariableOperators();

    return $this->variableObjectOperatorId;

  }

  public function doFindVariableOperators(): bool {

    if ($this->didIsVariableWithOperator == true) {
      return true;
    }

    $this->didIsVariableWithOperator = true;

    $tokens = $this->tokenStream()->tokens();

    for ($i = $this->getId(); $i < $tokens->count(); $i++) {

      $token = $tokens->get($i);

      if (!$token instanceof TokenInterface) {
        break;
      }

      if ($token instanceof PHP_Token_Whitespace) {
        continue;
      }

      if ($token->getTokenType() == Types::T_OPERATOR &&
          !$token instanceof PHP_Token_Object_Operator) {
        // JEO: This captures operators that are not a object operator:
        // $foo = ..;
        // $foo .= ..;
        $this->variableOperatorId = $token->getId();
        break;
      }

      // This block adds support for $objectFoo->somekindaaction...;
      // Nice a nullsafe operator, skip it.
      if ($token instanceof PHP_Token_Nullsafe_Object_Operator) {
        continue;
      }

      // Found a object operator capture it.
      if ($token instanceof PHP_Token_Object_Operator) {
        $this->variableObjectOperatorId = $token->getId();
        break;
      }

      // --
      // Unknown token encountered, therefor it's not a opeartor based assignment
      // moment.
      // --
      break;

    }

    return true;
  }

}
