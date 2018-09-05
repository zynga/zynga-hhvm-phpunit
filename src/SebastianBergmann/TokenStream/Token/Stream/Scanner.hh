<?hh // strict

namespace SebastianBergmann\TokenStream\Token\Stream;

use SebastianBergmann\TokenStream\Token\Factory as TokenFactory;

use Zynga\Framework\Dynamic\V1\DynamicClassCreation;
use SebastianBergmann\TokenStream\Token\Stream;
use SebastianBergmann\TokenStream\TokenInterface;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Comment;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Doc_Comment;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Halt_Compiler;
use SebastianBergmann\TokenStream\Token\CustomTokens;

use Zynga\CodeBase\V1\File;
use Zynga\CodeBase\V1\File\RawToken;

use \Exception;

class Scanner {

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

    for ($i = 0; $i < $numTokens; ++$i) {

      $token = $tokens->get($i);

      $newToken = null;
      $text = '';
      $skip = 0;

      if (!$token instanceof RawToken) {
        continue;
      }

      $text = $token->getText();
      $tokenId = $token->getTokenId();

      // first look up our custom tokens, as this is a unresolved token.
      if ($token->getLineNo() == -1) {

        $t_tokenName = CustomTokens::getTokenClassNameFromId($tokenId);

        if ($t_tokenName !== null) {
          $newToken = TokenFactory::createTokenFromName($t_tokenName);
        }

      }

      if ($newToken == null) {

        if ($lastNonWhitespaceTokenWasDoubleColon && $tokenId == T_CLASS) {
          $tokenClass = 'PHP_Token_Class_Name_Constant';
          $newToken = TokenFactory::createTokenFromName($tokenClass);
        } else if ($tokenId == T_USE &&
                   $tokens->containsKey($i + 2) &&
                   $tokens[$i + 2]->getTokenId() == T_FUNCTION) {
          $tokenClass = 'PHP_Token_Use_Function';
          $newToken = TokenFactory::createTokenFromName($tokenClass);
          $skip = 2;
        } else {
          // First attempt to pull the token out of the factory.
          $newToken = TokenFactory::createTokenFromTokenId($tokenId);

          if ($newToken == null) {
            // convert the raw token name into a class name
            $shortName = $this->resolveTokenIdToShortName($tokenId);
            $tokenClass = 'PHP_Token_'.$shortName;
            $newToken = TokenFactory::createTokenFromName($tokenClass);
          }

        }

      }

      // if ($newToken === null) {
      //   // --
      //   // JEO: DynamicClassCreation is actually somewhat slow with the number
      //   // of ops we are asking it to do.
      //   // --
      //   $tokenParams = Vector {};
      //
      //   $newToken = DynamicClassCreation::createClassByNameGeneric(
      //     'SebastianBergmann\TokenStream\Tokens\\'.$tokenClass,
      //     $tokenParams,
      //   );
      //
      // }

      $id++;

      if ($newToken instanceof TokenInterface) {
        $newToken->setAllAttributes($text, $line, $codeFile, $id);
        $stream->tokens()->add($newToken);
      }

      $lines = substr_count($text, "\n");
      $line += $lines;

      if ($newToken instanceof PHP_Token_Halt_Compiler) {
        break;
      }

      if ($tokenId == T_DOUBLE_COLON) {
        $lastNonWhitespaceTokenWasDoubleColon = true;
      } else if ($tokenId != T_WHITESPACE) {
        $lastNonWhitespaceTokenWasDoubleColon = false;
      }

      $i += $skip;
    }

  }

  private Map<int, string> $_tokenIdToShortName = Map {};

  protected function resolveTokenIdToShortName(int $tokenId): string {

    $shortName = $this->_tokenIdToShortName->get($tokenId);

    if ($shortName !== null) {
      return $shortName;
    }

    // strip the T_ off the token name before proceeding.
    $tokenName = substr($this->resolveTokenName($tokenId), 2);

    // explode the name into chunks
    $nameParts = explode("_", $tokenName);

    // make up a short name using the chunks provided
    $shortName = '';
    foreach ($nameParts as $namePart) {
      if ($shortName != '') {
        $shortName .= '_';
      }
      $shortName .= ucfirst(strtolower($namePart));
    }

    $this->_tokenIdToShortName->set($tokenId, $shortName);

    return $shortName;

  }

  protected function resolveTokenName(int $tokenId): string {
    // resolve using the built in token_name function.
    $tokenName = token_name($tokenId);
    return $tokenName;

  }

}
