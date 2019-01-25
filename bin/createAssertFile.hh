#!/usr/local/bin/hhvm
<?hh

$assertName = $argv[1];

$assertClass = $assertName;
$assertClass = str_replace('assert', 'Assert', $assertClass);

$assertFile = "./src/SebastianBergmann/PHPUnit/Assertions/" . $assertClass . ".hh";

echo "assertName: $assertName\n";
echo "assertFile: $assertFile\n";

if ( ! is_file($assertFile) ) {

  $fh = fopen($assertFile, 'w');

  fputs($fh, "<?hh // strict\n");
  fputs($fh, "\n");
  fputs($fh, "namespace SebastianBergmann\\PHPUnit\\Assertions;\n");
  fputs($fh, "\n");
  fputs($fh, "use SebastianBergmann\\PHPUnit\\Assertions;\n");
  fputs($fh, "\n");
  fputs($fh, "class $assertClass {\n");
  fputs($fh, "}\n");

  fclose($fh);

  echo "Wrote file: $assertFile\n";
  exit(0);

} else {

  echo "Refusing to overwrite=$assertFile\n";
  exit(255);

}
