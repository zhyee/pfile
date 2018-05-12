<?php
$fp = rockfile_fopen("test2.txt", "ab");

var_dump(rockfile_fwrite($fp, "少小离家老大回，乡音无改鬓毛衰。儿童相见不相识，笑问客从何处来。\n"));

rockfile_fclose($fp);
