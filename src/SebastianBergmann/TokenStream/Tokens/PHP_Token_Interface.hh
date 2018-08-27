<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenWithScopeAndVisibility;

class PHP_Token_Interface extends TokenWithScopeAndVisibility {

  /**
   * @var array|bool
   */
  protected Vector<string> $interfaces = Vector {};
  protected bool $didInterfaces = false;

  /**
   * @return string
   */
  public function getName(): string {
    $id = $this->getId();
    $tokens = $this->tokenStream()->tokens();
    $nameToken = $tokens->get($id + 2);
    if ($nameToken instanceof PHP_Token_String) {
      return strval($nameToken);
    }
    return '[UNKNOWN_INTERFACE]';
  }

  /**
   * @return bool
   */
  public function hasParent(): bool {
    $id = $this->getId();
    $tokens = $this->tokenStream()->tokens();
    $token = $tokens->get($id + 4);
    return $token instanceof PHP_Token_Extends;
  }

  /**
   * @return array
   */
  public function getPackage(): Map<string, string> {

    $tokens = $this->tokenStream()->tokens();

    $className = $this->getName();
    $docComment = $this->getDocblock();

    $result = Map {
      'namespace' => '',
      'fullPackage' => '',
      'category' => '',
      'package' => '',
      'subpackage' => '',
    };

    for ($i = $this->getId(); $i; --$i) {
      $token = $tokens->get($i);
      if ($token instanceof PHP_Token_Namespace) {
        $result->set('namespace', $token->getName());
        break;
      }
    }

    $categoryMatches = array();
    if (preg_match(
          '/@category[\s]+([\.\w]+)/',
          $docComment,
          $categoryMatches,
        )) {
      $result->set('category', strval($categoryMatches[1]));
    }

    $packageMatches = array();
    if (preg_match(
          '/@package[\s]+([\.\w]+)/',
          $docComment,
          $packageMatches,
        )) {
      $result->set('package', strval($packageMatches[1]));
      $result->set('fullPackage', strval($packageMatches[1]));
    }

    $subPackageMatches = array();
    if (preg_match(
          '/@subpackage[\s]+([\.\w]+)/',
          $docComment,
          $subPackageMatches,
        )) {
      $result->set('subpackage', strval($subPackageMatches[1]));
      $currentFullPackage = $result->get('fullPackage');
      $result->set(
        'fullPackage',
        strval($currentFullPackage).'.'.strval($subPackageMatches[1]),
      );
    }

    if (array_key_exists('fullPackage', $result) !== true) {
      $result->set(
        'fullPackage',
        $this->arrayToName(
          explode('_', str_replace('\\', '_', $className)),
          '.',
        ),
      );
    }

    return $result;
  }

  /**
   * @param array  $parts
   * @param string $join
   *
   * @return string
   */
  protected function arrayToName(
    array<string> $parts,
    string $join = '\\',
  ): string {
    $result = '';

    if (count($parts) > 1) {
      array_pop($parts);

      $result = implode($join, $parts);
    }

    return $result;
  }

  /**
   * @return bool|string
   */
  public function getParent(): mixed {

    if (!$this->hasParent()) {
      return false;
    }

    $i = $this->getId() + 6;
    $tokens = $this->tokenStream()->tokens();
    $className = (string) $tokens[$i];

    for ($r = $i; $r < $tokens->count(); $r++) {
      $token = $tokens->get($r);
      $nextToken = $tokens->get($r + 1);
      if ($nextToken === null || $nextToken instanceof PHP_Token_Whitespace) {
        break;
      }
      $className .= $token;
    }

    return $className;

  }

  /**
   * @return bool
   */
  public function hasInterfaces(): bool {

    $id = $this->getId();

    // basic: class implements X {}
    $tokens = $this->tokenStream()->tokens();

    $basicImplementsToken = $tokens->get($id + 4);

    if ($basicImplementsToken instanceof PHP_Token_Implements) {
      return true;
    }

    $deeperPokeToken = $tokens->get($id + 8);

    if ($deeperPokeToken instanceof PHP_Token_Implements) {
      return true;
    }

    return false;

  }

  /**
   * @return array|bool
   */
  public function getInterfaces(): mixed {

    if ($this->didInterfaces === true) {
      if ($this->interfaces->count() === 0) {
        return false;
      }
      return $this->interfaces;
    }

    if (!$this->hasInterfaces()) {
      $this->didInterfaces = true;
      return false;
    }

    $id = $this->getId();

    $tokens = $this->tokenStream()->tokens();

    $interfacesToken = $tokens->get($id + 4);

    $i = 0;

    if ($interfacesToken instanceof PHP_Token_Implements) {
      $i = $id + 3;
    } else {
      $i = $id + 7;
    }

    for ($r = $i; $r < $tokens->count(); $r++) {

      $token = $tokens->get($r);
      $nextToken = $tokens->get($r + 1);

      if ($nextToken instanceof PHP_Token_Open_Curly) {
        break;
      }

      if ($token instanceof PHP_Token_String) {
        $this->interfaces->add(strval($token));
      }

    }

    $this->didInterfaces = true;
    return $this->interfaces;
  }
}
