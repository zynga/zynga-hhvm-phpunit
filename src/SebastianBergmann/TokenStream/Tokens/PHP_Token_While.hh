<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenWithScopeStartsWithCurly;
use SebastianBergmann\TokenStream\Token\Types;
use SebastianBergmann\TokenStream\TokenInterface;

class PHP_Token_While extends TokenWithScopeStartsWithCurly {
  private bool $isDoWhile = false;
  private bool $do_didEndTokenId = false;
  private int $do_endTokenId = -1;

  public function isDoWhile(): bool {
    return $this->isDoWhile;
  }

  public function setIsDoWhile(bool $tf): bool {
    $this->isDoWhile = $tf;
    return true;
  }

  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }

  public function getShortTokenName(): string {
    return 'While';
  }

  public function getEndOfDefinitionTokenId(): int {

    // If this isn't a do while, we hunt for {
    if ( $this->isDoWhile() === false ) {
      return parent::getEndOfDefinitionTokenId();
    }

    // Otherwise in a do while the end of the def is the ;
    return $this->getEndTokenId();

  }

  public function getEndTokenId(): int {

    // If this isn't a do while, we hunt for }
    if ( $this->isDoWhile() === false ) {
      return parent::getEndTokenId();
    }

    // Otherwhise we hunt for a ;
    if ( $this->do_didEndTokenId === true ) {
      return $this->do_endTokenId;
    }

    $this->do_didEndTokenId = true;

    // --
    // This use case is a litle bizaro since the block is
    // inverted from the others.
    // --
    // do { ... something ... } while (... conditions );
    //                          ^ You are here.
    // --
    $startAt = $this->getId();

    $tokens = $this->tokenStream()->tokens();

    $tokenCount = $tokens->count();

    $foundWhile = false;
    for ( $i = $startAt; $i < $tokenCount; $i++ ) {
      $token = $tokens->get($i);

      if ( ! $token instanceof TokenInterface ) {
        break;
      }

      if ( $token instanceof PHP_Token_Semicolon ) {
        $this->do_endTokenId = $token->getId();
        break;
      }

    }

    return $this->do_endTokenId;

  }

}
