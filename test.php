<?php
print_r(get_extension_funcs('rockfile'));

$fp = rockfile_fopen("test.log", "ab");
$fp2 = rockfile_fopen("test.log", "ab");  //复用 $fp
$fp3 = rockfile_fopen("test.log", "rb");  //another open

var_dump(rockfile_fwrite($fp, "hello world....\n"));
var_dump(rockfile_fwrite($fp2, "hello world....\n", 5));
var_dump(rockfile_fwrite($fp, "hello world....\n", 9999));

echo rockfile_fread($fp3, 8) . "\n";
echo rockfile_fread($fp3, 999) . "\n";

rockfile_fclose($fp);  // 关闭文件句柄
rockfile_fclose($fp2);

