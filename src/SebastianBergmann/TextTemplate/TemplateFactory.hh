<?hh // strict

namespace SebastianBergmann\TextTemplate;

use SebastianBergmann\TextTemplate\Template;

class TemplateFactory {
  static private Map<string, Template> $templates = Map {};

  static public function get(
    string $filename,
    string $openDelimiter,
    string $closeDelimiter,
  ): Template {

    $template = self::$templates->get($filename);

    if ($template instanceof Template) {
      $template->setOpenDelimiter($openDelimiter);
      $template->setCloseDelimiter($closeDelimiter);
      return $template;
    }

    $template = new Template($filename, $openDelimiter, $closeDelimiter);

    self::$templates->set($filename, $template);

    return $template;

  }

}
