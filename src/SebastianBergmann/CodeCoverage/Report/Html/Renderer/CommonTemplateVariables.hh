<?hh // strict

namespace SebastianBergmann\CodeCoverage\Report\Html\Renderer;

class CommonTemplateVariables {
  public string $id;
  public string $full_path;
  public string $path_to_root;
  public string $breadcrumbs;
  public string $date;
  public string $version;
  public string $runtime;
  public string $generator;
  public string $low_upper_bound;
  public string $high_lower_bound;

  public function __construct() {
    $this->id = '';
    $this->full_path = '';
    $this->path_to_root = '';
    $this->breadcrumbs = '';
    $this->date = '';
    $this->version = '';
    $this->runtime = '';
    $this->generator = '';
    $this->low_upper_bound = '';
    $this->high_lower_bound = '';
  }

  public function asMap(): Map<string, mixed> {
    $data = Map {};
    $data->set('id', $this->id);
    $data->set('full_path', $this->full_path);
    $data->set('path_to_root', $this->path_to_root);
    $data->set('breadcrumbs', $this->breadcrumbs);
    $data->set('date', $this->date);
    $data->set('version', $this->version);
    $data->set('runtime', $this->runtime);
    $data->set('generator', $this->generator);
    $data->set('low_upper_bound', $this->low_upper_bound);
    $data->set('high_lower_bound', $this->high_lower_bound);
    return $data;
  }

}
