<?hh // strict

namespace Zynga\PHPUnit\V2;

use Zynga\PHPUnit\V2\RunnerPartialShim;
use Zynga\PHPUnit\V2\Version;

use \Exception;
use \PHPUnit_TextUI_Command;

// --
// JEO: Why not use commandline base?
// Because the eventual phpunit instanciations don't play well with it.
// --
class Runner {
  private string $projectRoot;
  private string $userName;
  private Vector<string> $argv;

  private string $htmlDir;

  public function __construct(
    string $projectRoot,
    string $userName,
    mixed $argv,
  ) {

    $this->projectRoot = $projectRoot;
    $this->userName = $userName;

    $this->argv = Vector {};

    if (is_array($argv)) {
      foreach ($argv as $arg) {
        $this->argv->add(strval($arg));
      }
    }

    $this->htmlDir = '';

  }

  public function run(): int {

    try {

      $this->message('Zynga-PHPUnit-Runner Version: ' . Version::get());

      $this->init();

      $argStack = $this->createPHPUnitArgStack();

      return RunnerPartialShim::runPHPUnit(true, $argStack->toArray());

    } catch (Exception $e) {
      return $this->failure($e->getMessage());
    }

    return 0;

  }

  public function createPHPUnitArgStack(): Vector<string> {
    $argStack = Vector {};

    $programName = $this->argv->get(0);

    if (is_string($programName)) {
      $argStack->add($programName);
    }


    // $argStack->add('--debug');
    // $argStack->add('--stop-on-failure');
    $argStack->add('--coverage-html='.$this->htmlDir);
    $argStack->add('--coverage-text');
    $argStack->add(
      '--configuration='.$this->projectRoot.'/phpunit.xml',
    );

    for ($i = 1; $i < $this->argv->count(); $i++) {

      $arg = $this->argv->get($i);

     if (is_string($arg)) {
       $argStack->add($arg);
     }

    }

    return $argStack;
  }

  public function init(): void {

    // Is our project root writeable?
    if (!is_writeable($this->projectRoot)) {
      throw new Exception(
        'projectRoot='.$this->projectRoot.' is not writable',
      );
    }

    $this->setupTmpDir();

    $coverageFile =
      $this->projectRoot.'/tmp/'.$this->userName.'-hh-phpunit.coverage';
    $this->htmlDir =
      $this->projectRoot.'/tmp/'.$this->userName.'-hh-phpunit-html';

    if (!is_dir($this->htmlDir)) {
      mkdir($this->htmlDir, 0755, true);
    }

    if (!is_dir($this->htmlDir) || !is_writeable($this->htmlDir)) {
      throw new Exception(
        'codeCoverageHtmlDir='.$this->htmlDir.' is not writeable',
      );
    }

    // now we force our testErrors into our error_log file
    $testErrorLogFile = $this->projectRoot.'/tmp/php.log';

    ini_set('error_log', $testErrorLogFile);

    $errorLogFile = ini_get('error_log');

    if ($errorLogFile != $testErrorLogFile) {
      throw new Exception(
        'failedErrorLogSetup expected='.
        $testErrorLogFile.
        ' have='.
        $errorLogFile,
      );
    }

    $currentTimezone = ini_get('date.timezone');

    if ($currentTimezone == '') {
      ini_set('date.timezone', 'UTC');

      $currentTimezone = ini_get('date.timezone');
      if ( $currentTimezone != 'UTC' ) {
        throw new Exception('failedTimeZoneCheck expected=UTC have=' . $currentTimezone);
      }
    }

  }

  public function getTmpDir(): string {
    return $this->projectRoot.'/tmp';
  }

  public function setupTmpDir(): void {

    $tmpDir = $this->getTmpDir();

    if (!is_dir($tmpDir)) {
      mkdir($tmpDir, 0755);
      if (!is_dir($tmpDir)) {
        throw new Exception('failedToCreateTmpDir='.$tmpDir);
      }
    }

    if (!is_writeable($tmpDir)) {
      throw new Exception('failedWrieableCheckForTmpDir='.$tmpDir);
    }

  }

  public function failure(string $message): int {
    $this->message('FAILURE - '.$message);
    return 255;
  }

  public function message(string $message): void {
    print date('r').' - '.$message."\n";
  }

}
