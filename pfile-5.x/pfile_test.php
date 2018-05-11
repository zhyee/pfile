<?php

print_r(get_extension_funcs("pfile"));

$fp1 = pfile_fopen("pfile.text", "ab");
var_dump($fp1);

$fp2 = &$fp1;

unset($fp2);
unset($fp1);

$fp = pfile_fopen("pfile.text", "ab");
//unset($fp2);
//unset($fp1);
//pfile_fclose($fp1);


var_dump(pfile_fwrite($fp, "hello pfile..."));
var_dump(pfile_fwrite($fp, "hello pfile...", 5));
var_dump(pfile_fwrite($fp, "hello pfile...", 1000));
