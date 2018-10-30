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

use SebastianBergmann\CodeCoverage\Driver;
use SebastianBergmann\CodeCoverage\Node\File as FileNode;
use SebastianBergmann\CodeCoverage\Report\Html\Renderer;
use SebastianBergmann\CodeCoverage\Report\Html\Renderer\Template\CodeFile;
use
  SebastianBergmann\CodeCoverage\Report\Html\Renderer\Template\CodeFileItem
;
use SebastianBergmann\CodeCoverage\Report\Html\Renderer\Template\CodeLine;
use SebastianBergmann\CodeCoverage\Report\Html\Renderer\Template\CodeRow;
use SebastianBergmann\CodeCoverage\Report\Html\Renderer\Template\CodeTokens;
use SebastianBergmann\CodeCoverage\Util;

use SebastianBergmann\TokenStream\Token\StreamInterfaceStructure;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Comment;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Catch;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Do;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Double_Colon;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Equal;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Finally;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_If;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Else;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Elseif;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Foreach;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Function;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Object_Operator;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Switch;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Try;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_While;
use SebastianBergmann\TokenStream\Tokens\PHP_Token_Variable;
use SebastianBergmann\TokenStream\Token\Types;
use
  SebastianBergmann\CodeCoverage\Report\Html\Renderer\SourcreFileLineBuffer
;

use Zynga\CodeBase\V1\FileFactory;
use Zynga\CodeBase\V1\Code\Code_Class;
use Zynga\CodeBase\V1\Code\Code_Method;
use SebastianBergmann\CodeCoverage\Node\Directory as DirectoryNode;

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
    DirectoryNode $root,
  ) {
    parent::__construct(
      $templatePath,
      $generator,
      $date,
      $lowUpperBound,
      $highLowerBound,
      $root,
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

    $templateVariables = $this->getCommonTemplateVariables($node);

    CodeFile::renderTo(
      $file,
      $templateVariables,
      $this->renderItems($node),
      $this->renderSource($node),
    );

  }

  /**
   * @param FileNode $node
   *
   * @return string
   */
  protected function renderItems(FileNode $node): string {

    $name = 'Total';

    $items = $this->renderFileItemTemplate(
      $name,
      $node->getNumClassesAndTraits(),
      $node->getNumTestedClassesAndTraits(),
      $node->getTestedClassesAndTraitsPercent(),
      $node->getTestedClassesAndTraitsPercentAsString(),
      $node->getNumMethods(),
      $node->getNumTestedMethods(),
      $node->getTestedMethodsPercent(),
      $node->getTestedMethodsPercentAsString(),
      $node->getNumExecutableLines(),
      $node->getNumExecutedLines(),
      $node->getLineExecutedPercent(),
      $node->getLineExecutedPercentAsString(),
      $node->getCcnAsString(),
      $node->getCrapAsString(),
    );

    $items .= $this->renderFunctionItems($node->getFunctions());

    $items .= $this->renderTraitOrClassItems($node->getTraits());

    $items .= $this->renderTraitOrClassItems($node->getClasses());

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
    Map<string, Code_Class> $items,
  ): string {

    if ($items->count() == 0) {
      return '';
    }

    $buffer = '';

    foreach ($items as $name => $classObj) {

      $classObj->calculateCoverage();

      $numClasses = 1;
      $numMethods = $classObj->getNumMethods();
      $numTestedMethods = $classObj->getNumTestedMethods();
      $numTestedClasses = $numTestedMethods == $numMethods ? 1 : 0;
      $testedClassesPercent = Util::percent($numTestedMethods, $numMethods);
      $testedMethodsPercent = Util::percent($numTestedMethods, $numMethods);
      $testedMethodsPercentAsString =
        Util::percentAsString($numTestedMethods, $numMethods);
      $executableLines = $classObj->getExecutableLines();
      $executedLines = $classObj->getExecutedLines();
      $linesExecutedPercent = Util::percent(
        $classObj->getExecutedLines(),
        $classObj->getExecutableLines(),
      );
      $testedClassesPercentAsString = Util::percentAsString(
        $numTestedMethods == $numMethods ? 1 : 0,
        1,
        true,
      );
      $linesExecutedPercentAsString = Util::percentAsString(
        $classObj->getExecutedLines(),
        $classObj->getExecutableLines(),
      );

      $buffer .= $this->renderFileItemTemplate(
        $name, // name
        $numClasses, // numClasses
        $numTestedClasses, // numTestedClasses
        $testedClassesPercent, // testedClassesPercent
        $testedClassesPercentAsString, // testedClassesPercentAsString
        $numMethods, // numMethods
        $numTestedMethods, // numTestedMethods
        $testedMethodsPercent, // testedMethodsPercent
        $testedMethodsPercentAsString, // testedMethodsPercentAsString
        $executableLines, // executableLines
        $executedLines, // executedLines
        $linesExecutedPercent, // linesExecutedPercent
        $linesExecutedPercentAsString,
        $classObj->getCcnAsString(),
        $classObj->getCrapAsString(),
      );

      foreach ($classObj->methods as $method) {
        $buffer .= $this->renderFunctionOrMethodItem($method, '&nbsp;');
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
    Map<string, Code_Method> $functions,
  ): string {

    if ($functions->count() == 0) {
      return '';
    }

    $buffer = '';

    foreach ($functions as $function) {
      $buffer .= $this->renderFunctionOrMethodItem($function);
    }

    return $buffer;
  }

  /**
   * @param \Text_Template $template
   *
   * @return string
   */
  protected function renderFunctionOrMethodItem(
    Code_Method $methodObj,
    string $indent = '',
  ): string {

    $numTestedItems =
      $methodObj->getExecutedLines() == $methodObj->getExecutableLines()
        ? 1
        : 0;

    $methodObj->calculateCoverage();

    $name = sprintf(
      '%s<a href="#%d"><abbr title="%s">%s</abbr></a>',
      $indent,
      $methodObj->startLine,
      htmlspecialchars($methodObj->signature),
      $methodObj->methodName,
    );

    $testedMethodsPercent = Util::percent($numTestedItems, 1);
    $testedMethodsPercentAsString = Util::percentAsString($numTestedItems, 1);

    $linesExecutedPercent = Util::percent(
      $methodObj->getExecutedLines(),
      $methodObj->getExecutableLines(),
      false,
    );

    $linesExecutedPercentAsString = Util::percentAsString(
      $methodObj->getExecutedLines(),
      $methodObj->getExecutableLines(),
    );

    return $this->renderFileItemTemplate(
      $name, // name
      0, // numClasses
      0, // numTestedClasses
      0.0, // testedClassesPercent
      '', // testedClassesPercentAsString
      1, // numMethods
      $numTestedItems, // numTestedMethods
      $testedMethodsPercent, // testedMethodsPercent
      $testedMethodsPercentAsString, // testedMethodsPercentAsString
      $methodObj->getExecutableLines(), // numExecutableLines
      $methodObj->getExecutedLines(), // numExecutedLines
      $linesExecutedPercent, // linesExecutedPercent
      $linesExecutedPercentAsString,
      $methodObj->getCcnAsString(),
      $methodObj->getCrapAsString(),
    );
  }

  const DEBUG_TOKENS = true;

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

    $codeLines = $this->loadFile($fileName);

    $processedFile = FileFactory::get($fileName);

    $lines = '';

    $lineNo = 0;

    foreach ($codeLines as $codeLine) {

      $lineNo++;

      $lineStateValue = $coverageData->get($lineNo);

      // @TODO: move all the tr creation into it's own function
      // @TODO: move popover creation into it's own function
      $trClass = '';
      $popover = '';

      $lineStateCss = '';
      $lineState = '';

      if ($lineStateValue == Driver::LINE_EXECUTED) {
        $lineStateCss = 'success';
        $lineState = '&check;';
      } else if ($lineStateValue == Driver::LINE_NOT_EXECUTED) {
        $lineStateCss = 'danger';
        $lineState = '&otimes;';
      } else if ($lineStateValue == Driver::LINE_NOT_EXECUTABLE) {
        $lineStateCss = 'info';
        $lineState = '&nbsp;';
      }

      if (preg_match('/\@codeCoverageIgnore/', $codeLine)) {
        $lineStateCss = 'danger';
        $lineState = 'NA';
        $codeLine .=
          '<span class="string">!!We do not support code coverage ignore.!!</span>';
      }

      $lines .= CodeRow::render(
        $lineNo,
        $trClass,
        $popover,
        $lineStateCss,
        $lineState,
        $codeLine,
      );

      // if we are debugging tokens and what it means for the rendering.
      if (self::DEBUG_TOKENS === true) {
        $tokensForLine = '';

        $tokens = $processedFile->stream()->getLineToTokens($lineNo);

        foreach ($tokens as $token) {

          if ($tokensForLine != '') {
            $tokensForLine .= ',&nbsp;';
          }

          $tokensForLine .= $token->getShortTokenName();
          $tokensForLine .= '[tokenId|'.$token->getId().']';

          if ($token instanceof PHP_Token_Foreach ||
              $token instanceof PHP_Token_Do ||
              $token instanceof PHP_Token_While ||
              $token instanceof PHP_Token_Function ||
              $token instanceof PHP_Token_If ||
              $token instanceof PHP_Token_Else ||
              $token instanceof PHP_Token_Elseif ||
              $token instanceof PHP_Token_Switch ||
              $token instanceof PHP_Token_Try ||
              $token instanceof PHP_Token_Catch ||
              $token instanceof PHP_Token_Finally ||
              $token instanceof PHP_Token_Object_Operator ||
              $token instanceof PHP_Token_Double_Colon ||
              $token instanceof PHP_Token_Variable ||
              $token instanceof PHP_Token_Equal ||
              $token->getTokenType() == Types::T_OPERATOR) {

            $tokensForLine .= '::[endTokenId|'.$token->getEndTokenId().']';
            $tokensForLine .= '::[endLineNo|'.$token->getEndLine().']';
            $tokensForLine .=
              '::[endOfDefinitionId|'.$token->getEndOfDefinitionTokenId().']';
            $tokensForLine .=
              '::[endOfDefinitionLineNo|'.
              $token->getEndOfDefinitionLineNo().
              ']';

            if ($token instanceof PHP_Token_If ||
                $token instanceof PHP_Token_Elseif ||
                $token instanceof PHP_Token_Else) {

              $tokensForLine .=
                '::[hasContinuation|'.
                json_encode($token->hasContinuation()).
                ']';
              $tokensForLine .=
                '::[continuationId|'.$token->getContinuationTokenId().']';

            }

            if ( $token instanceof PHP_Token_Variable) {
              $tokensForLine .=
                '::[getVariableWithOperatorTokenId|'. $token->getVariableWithOperatorTokenId() .']';
              $tokensForLine .=
                '::[getVariableWithObjectOperatorTokenId|' . $token->getVariableWithObjectOperatorTokenId() . ']';
            }

          }

        }

        $lines .= CodeTokens::render('Tokens', $lineNo, $tokensForLine);

        // Executable ranges need debugging at times.
        $execRanges =
          $processedFile->lineExecutionState()->getExecutableRanges($lineNo);

        if ($execRanges instanceof Vector && $execRanges->count() > 0) {
          $data = '';
          foreach ($execRanges as $range) {
            if ($data != '') {
              $data .= ', ';
            }
            $data .=
              $range->getReason().
              ' ['.
              $range->getStart().
              ':'.
              $range->getEnd().
              ']';
          }
          $lines .= CodeTokens::render('ExecRanges', $lineNo, $data);
        } else {
          $lines .= CodeTokens::render('ExecRanges', $lineNo, '-NONE-');
        }

      }

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
  protected function loadFile(string $file): array<string> {

    if (FileFactory::isFileRegistered($file) !== true) {
      error_log(
        "WARNING: file=$file was not registered, having to load at report time\n",
      );
    }

    $processedFile = FileFactory::get($file);

    $buffer = $processedFile->source()->get();
    //$tokens = $processedFile->rawTokens()->get();
    $tokens = $processedFile->stream()->tokens();

    $result = new SourceFileLineBuffer();

    $i = 0;
    $stringFlag = false;

    foreach ($tokens as $j => $token) {

      $value = $token->getText();

      $value = htmlspecialchars($value, $this->htmlspecialcharsFlags);

      if ($value === "\n") {
        $result->set(++$i, '');
      } else {

        $lines = explode("\n", $value);

        $lineCount = count($lines);
        foreach ($lines as $jj => $line) {
          // $line = trim($line);

          if ($line !== '') {
            if ($stringFlag) {
              $colour = 'string';
            } else {
              $colour = $token->getTokenType();
            }

            //if ($colour == 'default') {
            //  $line = $token.$line;
            //}

            $result->appendTo($i, CodeLine::render($colour, $line));

          }

          if (($jj + 1) < $lineCount) {
            $result->set(++$i, '');
          }
        }
      }
    }

    return $result->get();

  }

  protected function renderFileItemTemplate(
    string $name,
    int $numClasses,
    int $numTestedClasses,
    float $testedClassesPercent,
    string $classesPercentAsString,
    int $numMethods,
    int $numTestedMethods,
    float $testedMethodsPercent,
    string $methodsPercentAsString,
    int $numExecutableLines,
    int $numExecutedLines,
    float $linesExecutedPercent,
    string $linesExecutedPercentAsString,
    string $ccn,
    string $crap,
  ): string {

    $numSeparator = '&nbsp;/&nbsp;';

    if ($numClasses > 0) {
      $classesLevel = $this->getColorLevel($testedClassesPercent);
      $classesNumber = $numTestedClasses.$numSeparator.$numClasses;
      $classesBar = $this->getCoverageBar($testedClassesPercent);
    } else {
      $classesLevel = 'success';
      $classesNumber = '0'.$numSeparator.'0';
      $classesBar = $this->getCoverageBar(100.00);
    }

    if ($numMethods > 0) {
      $methodsLevel = $this->getColorLevel($testedMethodsPercent);
      $methodsNumber = $numTestedMethods.$numSeparator.$numMethods;
      $methodsBar = $this->getCoverageBar($testedMethodsPercent);
    } else {
      $methodsLevel = 'success';
      $methodsNumber = '0'.$numSeparator.'0';
      $methodsBar = $this->getCoverageBar(100.00);
      $methodsPercentAsString = '100.00%';
    }

    if ($numExecutableLines > 0) {
      $linesLevel = $this->getColorLevel($linesExecutedPercent);
      $linesNumber = $numExecutedLines.$numSeparator.$numExecutableLines;
      $linesBar = $this->getCoverageBar($linesExecutedPercent);
    } else {
      $linesLevel = 'success';
      $linesNumber = '0'.$numSeparator.'0';
      $linesBar = $this->getCoverageBar(100.00);
      $linesExecutedPercentAsString = '100.00%';
    }

    return CodeFileItem::render(
      $name,
      $classesNumber,
      $classesLevel,
      $classesPercentAsString,
      $classesBar,
      $methodsNumber,
      $methodsLevel,
      $methodsPercentAsString,
      $methodsBar,
      $linesNumber,
      $linesLevel,
      $linesExecutedPercentAsString,
      $linesBar,
      $ccn,
      $crap,
    );

  }

}
