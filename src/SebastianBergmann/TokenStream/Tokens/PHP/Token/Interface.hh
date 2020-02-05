<?hh // strict

namespace SebastianBergmann\TokenStream\Tokens;

use SebastianBergmann\TokenStream\TokenWithScopeAndVisibility;
use SebastianBergmann\TokenStream\Token\Types;
use SebastianBergmann\TokenStream\TokenInterface;

class PHP_Token_Interface extends TokenWithScopeAndVisibility {

  /**
   * @var array|bool
   */
  private Vector<string> $interfaces = Vector {};
  private bool $didInterfaces = false;

  private bool $_didName = false;
  private string $_name = '[UNKNOWN_INTERFACE]';
  private bool $_didHasParent = false;
  private bool $_hasParent = false;
  private bool $_didGetParent = false;
  private string $_parentName = '';

  /**
   * @return string
   */
  public function getName(): string {

    if ($this->_didName == true) {
      return $this->_name;
    }

    $id = $this->getId();
    $tokens = $this->tokenStream()->tokens();
    for ($i = $id; $i < ($id + 10); $i++) {
      $nameToken = $tokens->get($i);
      if ($nameToken instanceof PHP_Token_String) {
        $this->_name = $nameToken->getText();
        break;
      }
    }
    $this->_didName = true;
    return $this->_name;
  }

  /**
   * @return bool
   */
  public function hasParent(): bool {

    if ($this->_didHasParent == true) {
      return $this->_hasParent;
    }

    $parent = $this->getParent();

    if ($parent != '') {
      $this->_hasParent = true;
    }

    $this->_didHasParent = true;
    return $this->_hasParent;

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

  public function getParent(): ?string {

    if ($this->_didGetParent == true) {
      return $this->_parentName;
    }

    $this->_hasParent = false;
    $this->_parentName = '';

    $id = $this->getId();

    $tokens = $this->tokenStream()->tokens();

    for ($i = $id; $i < ($id + 50); $i++) {

      $token = $tokens->get($i);

      if (!$token instanceof TokenInterface) {
        continue;
      }

      // hunt for the interface foo extends baz {}
      if ($token instanceof PHP_Token_Extends) {
        $this->_hasParent = true;
        continue;
      }

      // once the extend is found we have a parent and swap into hunting
      // for the class name and finally the open curly.
      if ($this->_hasParent == false) {
        continue;
      }

      // if we hit a open curly we are done.
      if ($token instanceof PHP_Token_Open_Curly) {
        break;
      }

      // ignore whitespace
      if ($token instanceof PHP_Token_Whitespace) {
        continue;
      }

      // bump the value of the token into the name
      $this->_parentName .= $token->getText();

    }

    $this->_didGetParent = true;
    return $this->_parentName;

  }

  /**
   * @return bool
   */
  public function hasInterfaces(): bool {

    $interfaces = $this->getInterfaces();

    if ($interfaces->count() > 0) {
      return true;
    }

    return false;

  }

  /**
   * @return array|bool
   */
  public function getInterfaces(): Vector<string> {

    if ($this->didInterfaces === true) {
      return $this->interfaces;
    }

    $id = $this->getId();

    $tokens = $this->tokenStream()->tokens();

    $foundImplements = false;
    for ($i = $id; $i < $tokens->count(); $i++) {

      $token = $tokens->get($i);

      // if we hit a open curly we're done
      if ($token instanceof PHP_Token_Open_Curly) {
        break;
      }

      // found a implements clause
      if ($token instanceof PHP_Token_Implements) {
        $foundImplements = true;
        continue;
      }

      // if no implements, don't process it for interfaces.
      if ($foundImplements === false) {
        continue;
      }

      // We found a string type, yay.. it's a implements Foo clause in this
      // context.
      if ($token instanceof PHP_Token_String) {
        $this->interfaces->add($token->getText());
      }
    }

    $this->didInterfaces = true;
    return $this->interfaces;
  }

  public function getTokenType(): string {
    return Types::T_KEYWORD;
  }

  public function getShortTokenName(): string {
    return 'Interface';
  }

}
