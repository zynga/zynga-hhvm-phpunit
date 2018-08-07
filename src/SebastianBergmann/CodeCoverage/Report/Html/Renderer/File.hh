<?hh // strict

/*
 * This file is part of the php-code-coverage package.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace SebastianBergmann\CodeCoverage\Report\Html\Renderer;

use SebastianBergmann\CodeCoverage\Node\File as FileNode;
use SebastianBergmann\CodeCoverage\ProcessedFile\FileContainer;
use SebastianBergmann\CodeCoverage\Report\Html\Renderer;
use SebastianBergmann\CodeCoverage\Util;
use SebastianBergmann\TextTemplate\TemplateFactory;
use SebastianBergmann\TextTemplate\Template;
use SebastianBergmann\TokenStream\Token\StreamClassStructure;
use SebastianBergmann\TokenStream\Token\StreamMethodStructure;
use SebastianBergmann\TokenStream\Token\StreamInterfaceStructure;

/**
 * Renders a file node.
 */
class File extends Renderer {
  /**
   * @var int
   */
  private int $htmlspecialcharsFlags;

  /**
   * Constructor.
   *
   * @param string $templatePath
   * @param string $generator
   * @param string $date
   * @param int    $lowUpperBound
   * @param int    $highLowerBound
   */
  public function __construct(
    string $templatePath,
    string $generator,
    string $date,
    int $lowUpperBound,
    int $highLowerBound,
  ) {
    parent::__construct(
      $templatePath,
      $generator,
      $date,
      $lowUpperBound,
      $highLowerBound,
    );

    $this->htmlspecialcharsFlags = 0;

    // --
    // JEO These constants have changed over the life of php, so please wrap them
    // like the following
    // --
    if (defined('ENT_COMPAT')) {
      $this->htmlspecialcharsFlags |= constant('ENT_COMPAT');
    }

    if (defined('ENT_HTML401')) {
      $this->htmlspecialcharsFlags |= constant('ENT_HTML401');
    }

    if (defined('ENT_SUBSTITUTE')) {
      $this->htmlspecialcharsFlags |= constant('ENT_SUBSTITUTE');
    }

  }

  /**
   * @param FileNode $node
   * @param string   $file
   */
  public function render(FileNode $node, string $file): void {

    $template =
      TemplateFactory::get($this->templatePath.'file.html', '{{', '}}');

    $templateVariables = $this->getCommonTemplateVariables($node);

    $templateVariables->set('items', $this->renderItems($node));
    $templateVariables->set('lines', $this->renderSource($node));

    $template->renderTo($file, $templateVariables);

  }

  /**
   * @param FileNode $node
   *
   * @return string
   */
  protected function renderItems(FileNode $node): string {
    $template =
      TemplateFactory::get($this->templatePath.'file_item.html', '{{', '}}');

    $methodItemTemplate = TemplateFactory::get(
      $this->templatePath.'method_item.html',
      '{{',
      '}}',
    );

    $items =
      $this->renderItemTemplate(
        $template,
        Map {
          'name' => 'Total',
          'numClasses' => $node->getNumClassesAndTraits(),
          'numTestedClasses' => $node->getNumTestedClassesAndTraits(),
          'numMethods' => $node->getNumMethods(),
          'numTestedMethods' => $node->getNumTestedMethods(),
          'linesExecutedPercent' => $node->getLineExecutedPercent(false),
          'linesExecutedPercentAsString' => $node->getLineExecutedPercent(),
          'numExecutedLines' => $node->getNumExecutedLines(),
          'numExecutableLines' => $node->getNumExecutableLines(),
          'testedMethodsPercent' => $node->getTestedMethodsPercent(false),
          'testedMethodsPercentAsString' =>
            $node->getTestedMethodsPercent(),
          'testedClassesPercent' =>
            $node->getTestedClassesAndTraitsPercent(false),
          'testedClassesPercentAsString' =>
            $node->getTestedClassesAndTraitsPercent(),
          'crap' =>
            '<abbr title="Change Risk Anti-Patterns (CRAP) Index">CRAP</abbr>',
        },
      );

    $items .=
      $this->renderFunctionItems($node->getFunctions(), $methodItemTemplate);

    $items .= $this->renderTraitOrClassItems(
      $node->getTraits(),
      $template,
      $methodItemTemplate,
    );

    $items .= $this->renderTraitOrClassItems(
      $node->getClasses(),
      $template,
      $methodItemTemplate,
    );

    return $items;
  }

  /**
   * @param array          $items
   * @param \Text_Template $template
   * @param \Text_Template $methodItemTemplate
   *
   * @return string
   */
  protected function renderTraitOrClassItems(
    Map<string, StreamClassStructure> $items,
    Template $template,
    Template $methodItemTemplate,
  ): string {

    if ($items->count() == 0) {
      return '';
    }

    $buffer = '';

    foreach ($items as $name => $classObj) {

      $numMethods = $classObj->numMethods;
      $numTestedMethods = $classObj->numTestedMethods;

      $buffer .= $this->renderItemTemplate(
        $template,
        Map {
          'name' => $name,
          'numClasses' => 1,
          'numTestedClasses' => $numTestedMethods == $numMethods ? 1 : 0,
          'numMethods' => $numMethods,
          'numTestedMethods' => $numTestedMethods,
          'linesExecutedPercent' => Util::percent(
            $classObj->executedLines,
            $classObj->executableLines,
            false,
          ),
          'linesExecutedPercentAsString' => Util::percent(
            $classObj->executedLines,
            $classObj->executableLines,
            true,
          ),
          'numExecutedLines' => $classObj->executedLines,
          'numExecutableLines' => $classObj->executableLines,
          'testedMethodsPercent' => Util::percent(
            $numTestedMethods,
            $numMethods,
            false,
          ),
          'testedMethodsPercentAsString' => Util::percent(
            $numTestedMethods,
            $numMethods,
            true,
          ),
          'testedClassesPercent' => Util::percent(
            $numTestedMethods == $numMethods ? 1 : 0,
            1,
            false,
          ),
          'testedClassesPercentAsString' => Util::percent(
            $numTestedMethods == $numMethods ? 1 : 0,
            1,
            true,
          ),
          'crap' => $classObj->crap,
        },
      );

      foreach ($classObj->methods as $method) {
        $buffer .= $this->renderFunctionOrMethodItem(
          $methodItemTemplate,
          $method,
          '&nbsp;',
        );
      }
    }

    return $buffer;
  }

  /**
   * @param array          $functions
   * @param \Text_Template $template
   *
   * @return string
   */
  protected function renderFunctionItems(
    Map<string, StreamMethodStructure> $functions,
    Template $template,
  ): string {

    if ($functions->count() == 0) {
      return '';
    }

    $buffer = '';

    foreach ($functions as $function) {
      $buffer .= $this->renderFunctionOrMethodItem($template, $function);
    }

    return $buffer;
  }

  /**
   * @param \Text_Template $template
   *
   * @return string
   */
  protected function renderFunctionOrMethodItem(
    Template $template,
    StreamMethodStructure $methodObj,
    string $indent = '',
  ): string {
    $numTestedItems =
      $methodObj->executedLines == $methodObj->executableLines ? 1 : 0;

    return $this->renderItemTemplate(
      $template,
      Map {
        'name' => sprintf(
          '%s<a href="#%d"><abbr title="%s">%s</abbr></a>',
          $indent,
          $methodObj->startLine,
          htmlspecialchars($methodObj->signature),
          $methodObj->methodName,
        ),
        'numMethods' => 1,
        'numTestedMethods' => $numTestedItems,
        'linesExecutedPercent' => Util::percent(
          $methodObj->executedLines,
          $methodObj->executableLines,
          false,
        ),
        'linesExecutedPercentAsString' => Util::percent(
          $methodObj->executedLines,
          $methodObj->executableLines,
          true,
        ),
        'numExecutedLines' => $methodObj->executedLines,
        'numExecutableLines' => $methodObj->executableLines,
        'testedMethodsPercent' => Util::percent($numTestedItems, 1, false),
        'testedMethodsPercentAsString' => Util::percent(
          $numTestedItems,
          1,
          true,
        ),
        'crap' => $methodObj->crap,
      },
    );
  }

  /**
   * @param FileNode $node
   *
   * @return string
   */
  protected function renderSource(FileNode $node): string {

    $fileName = $node->getPath();

    if (!is_string($fileName)) {
      return 'invalid fileName provided';
    }

    $coverageData = $node->getCoverageData();

    $testData = array();

    // $node->getTestData(); -- JEO need to wire in the test data.
    $codeLines = $this->loadFile($fileName);

    $lines = '';

    $lineNo = 0;

    foreach ($codeLines as $codeLine) {

      $lineNo++;

      // @TODO: move all the tr creation into it's own function
      // @TODO: move popover creation into it's own function
      $trClass = '';
      $popover = '';

      $lines .=
        sprintf(
          '     <tr%s%s><td><div align="right"><a name="%d"></a><a href="#%d">%d</a></div></td><td class="codeLine">%s</td></tr>'.
          "\n",
          $trClass,
          $popover,
          $lineNo,
          $lineNo,
          $lineNo,
          $codeLine,
        );

    }

    // $lines = '';
    // $i = 1;
    //
    // foreach ($codeLines as $line) {
    //   $trClass = '';
    //   $popoverContent = '';
    //   $popoverTitle = '';
    //
    //   if (array_key_exists($i, $coverageData)) {
    //     $numTests = count($coverageData[$i]);
    //
    //     if ($coverageData[$i] === null) {
    //       $trClass = ' class="warning"';
    //     } else if ($numTests == 0) {
    //       $trClass = ' class="danger"';
    //     } else {
    //       $lineCss = 'covered-by-large-tests';
    //       $popoverContent = '<ul>';
    //
    //       if ($numTests > 1) {
    //         $popoverTitle = $numTests.' tests cover line '.$i;
    //       } else {
    //         $popoverTitle = '1 test covers line '.$i;
    //       }
    //
    //       foreach ($coverageData[$i] as $test) {
    //         if ($lineCss == 'covered-by-large-tests' &&
    //             $testData[$test]['size'] == 'medium') {
    //           $lineCss = 'covered-by-medium-tests';
    //         } else if ($testData[$test]['size'] == 'small') {
    //           $lineCss = 'covered-by-small-tests';
    //         }
    //
    //         switch ($testData[$test]['status']) {
    //           case 0:
    //             switch ($testData[$test]['size']) {
    //               case 'small':
    //                 $testCSS = ' class="covered-by-small-tests"';
    //                 break;
    //
    //               case 'medium':
    //                 $testCSS = ' class="covered-by-medium-tests"';
    //                 break;
    //
    //               default:
    //                 $testCSS = ' class="covered-by-large-tests"';
    //                 break;
    //             }
    //             break;
    //
    //           case 1:
    //           case 2:
    //             $testCSS = ' class="warning"';
    //             break;
    //
    //           case 3:
    //             $testCSS = ' class="danger"';
    //             break;
    //
    //           case 4:
    //             $testCSS = ' class="danger"';
    //             break;
    //
    //           default:
    //             $testCSS = '';
    //         }
    //
    //         $popoverContent .=
    //           sprintf('<li%s>%s</li>', $testCSS, htmlspecialchars($test));
    //       }
    //
    //       $popoverContent .= '</ul>';
    //       $trClass = ' class="'.$lineCss.' popin"';
    //     }
    //   }
    //
    //   if (!empty($popoverTitle)) {
    //     $popover =
    //       sprintf(
    //         ' data-title="%s" data-content="%s" data-placement="bottom" data-html="true"',
    //         $popoverTitle,
    //         htmlspecialchars($popoverContent),
    //       );
    //   } else {
    //     $popover = '';
    //   }
    //
    //   $lines .=
    //     sprintf(
    //       '     <tr%s%s><td><div align="right"><a name="%d"></a><a href="#%d">%d</a></div></td><td class="codeLine">%s</td></tr>'.
    //       "\n",
    //       $trClass,
    //       $popover,
    //       $i,
    //       $i,
    //       $i,
    //       $line,
    //     );
    //
    //   $i++;
    // }

    return $lines;
  }

  /**
   * @param string $file
   *
   * @return array
   */
  protected function loadFile(string $file): Vector<string> {

    $processedFile = FileContainer::get($file);

    $fileBuffer = Vector {};

    $tokenStream = $processedFile->stream();
    $tokens = $tokenStream->tokens();

    $lineNo = -1;
    $lineBuffer = '';
    foreach ($tokens as $token) {

      $nextLineNo = $token->getLine();

      if ($lineNo != $nextLineNo) {
        if ($lineNo === -1) {
          // do nothing, as this is the first token on the stack.
        } else {
          // end of line
          $fileBuffer->add($lineBuffer);
          $lineBuffer = '';
          $lineNo = $nextLineNo;
        }
      }

      // @TODO: we need to put more logic here.
      $escapedToken =
        htmlspecialchars(strval($token), $this->htmlspecialcharsFlags);

      // @TODO: color determination
      $colour = 'keyword';
      $lineBuffer .=
        sprintf('<span class="%s">%s</span>', $colour, $escapedToken);

    }

    return $fileBuffer;

    // $htmlTemplate = Zynga_Source_Cache::getCodeCoverageRaw($file);
    //
    // if ($htmlTemplate !== null) {
    //   return $htmlTemplate;
    // }
    //
    // // echo "loading file=$file\n";
    // // JEO: This should already be in the token cache if available?
    // $buffer = Zynga_Source_Cache::getSource($file);
    // $tokens = Zynga_Source_Cache::getTokens($file);
    //
    // // JEO: Original code is here, caching version above.
    // // $buffer              = file_get_contents($file);
    // // $tokens              = token_get_all($buffer);
    // // -- End JEO mods
    // $result = [''];
    // $i = 0;
    // $stringFlag = false;
    // $fileEndsWithNewLine = substr($buffer, -1) == "\n";
    //
    // unset($buffer);
    //
    // foreach ($tokens as $j => $token) {
    //   if (is_string($token)) {
    //     if ($token === '"' && $tokens[$j - 1] !== '\\') {
    //       $result[$i] .= sprintf(
    //         '<span class="string">%s</span>',
    //         htmlspecialchars($token),
    //       );
    //
    //       $stringFlag = !$stringFlag;
    //     } else {
    //       $result[$i] .= sprintf(
    //         '<span class="keyword">%s</span>',
    //         htmlspecialchars($token),
    //       );
    //     }
    //
    //     continue;
    //   }
    //
    //   list($token, $value) = $token;
    //
    //   $value = str_replace(
    //     ["\t", ' '],
    //     ['&nbsp;&nbsp;&nbsp;&nbsp;', '&nbsp;'],
    //     htmlspecialchars($value, $this->htmlspecialcharsFlags),
    //   );
    //
    //   if ($value === "\n") {
    //     $result[++$i] = '';
    //   } else {
    //     $lines = explode("\n", $value);
    //
    //     foreach ($lines as $jj => $line) {
    //       $line = trim($line);
    //
    //       if ($line !== '') {
    //         if ($stringFlag) {
    //           $colour = 'string';
    //         } else {
    //           switch ($token) {
    //             case T_INLINE_HTML:
    //               $colour = 'html';
    //               break;
    //
    //             case T_COMMENT:
    //             case T_DOC_COMMENT:
    //               $colour = 'comment';
    //               break;
    //
    //             case T_ABSTRACT:
    //             case T_ARRAY:
    //             case T_AS:
    //             case T_BREAK:
    //             case T_CALLABLE:
    //             case T_CASE:
    //             case T_CATCH:
    //             case T_CLASS:
    //             case T_CLONE:
    //             case T_CONTINUE:
    //             case T_DEFAULT:
    //             case T_ECHO:
    //             case T_ELSE:
    //             case T_ELSEIF:
    //             case T_EMPTY:
    //             case T_ENDDECLARE:
    //             case T_ENDFOR:
    //             case T_ENDFOREACH:
    //             case T_ENDIF:
    //             case T_ENDSWITCH:
    //             case T_ENDWHILE:
    //             case T_EXIT:
    //             case T_EXTENDS:
    //             case T_FINAL:
    //             case T_FINALLY:
    //             case T_FOREACH:
    //             case T_FUNCTION:
    //             case T_GLOBAL:
    //             case T_IF:
    //             case T_IMPLEMENTS:
    //             case T_INCLUDE:
    //             case T_INCLUDE_ONCE:
    //             case T_INSTANCEOF:
    //             case T_INSTEADOF:
    //             case T_INTERFACE:
    //             case T_ISSET:
    //             case T_LOGICAL_AND:
    //             case T_LOGICAL_OR:
    //             case T_LOGICAL_XOR:
    //             case T_NAMESPACE:
    //             case T_NEW:
    //             case T_PRIVATE:
    //             case T_PROTECTED:
    //             case T_PUBLIC:
    //             case T_REQUIRE:
    //             case T_REQUIRE_ONCE:
    //             case T_RETURN:
    //             case T_STATIC:
    //             case T_THROW:
    //             case T_TRAIT:
    //             case T_TRY:
    //             case T_UNSET:
    //             case T_USE:
    //             case T_VAR:
    //             case T_WHILE:
    //             case T_YIELD:
    //               $colour = 'keyword';
    //               break;
    //
    //             default:
    //               $colour = 'default';
    //           }
    //         }
    //
    //         $result[$i] .=
    //           sprintf('<span class="%s">%s</span>', $colour, $line);
    //       }
    //
    //       if (isset($lines[$jj + 1])) {
    //         $result[++$i] = '';
    //       }
    //     }
    //   }
    // }
    //
    // if ($fileEndsWithNewLine) {
    //   unset($result[count($result) - 1]);
    // }
    //
    // Zynga_Source_Cache::setCodeCoverageRaw($file, $result);
    //
    // return $result;
  }
}
