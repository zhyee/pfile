# rockfile
常驻内存的PHP文件操作扩展，实现php-fpm多个请求之间能够复用打开的文件句柄，提升本地IO性能。
在PHP_FPM中，PHP自带的文件操作函数（fopen/file_put_contents）对于每一个打开的文件句柄，即使代码中没有手动关闭，请求结束时也会被自动销毁，对于记录日志等频繁需要操作同一个文件的场景来说，完全可以持久化打开的文件句柄，实现一次打开，之后各个请求复用这个打开的文件句柄，从而在一定程度上提升文件写入的效率。

## API
本扩展提供了以下函数，同PHP原生函数用法类似
* resource rockfile_fopen ( string $filename , string $mode )
* string rockfile_fread ( resource $handle , int $length )
* int rockfile_fwrite ( resource $handle , string $string \[, int $length \] )
* bool rockfile_fclose ( resource $handle )

## examples
```php
<?php
print_r(get_extension_funcs('rockfile'));

$fp1 = rockfile_fopen("test.log", "ab");
$fp2 = rockfile_fopen("test.log", "ab");  //参数完全相同会复用 $fp1打开的文件
$fp3 = rockfile_fopen("test.log", "rb");  //参数不同则会新打开文件

var_dump(rockfile_fwrite($fp1, "hello world....\n"));
var_dump(rockfile_fwrite($fp2, "hello world....\n", 5));
var_dump(rockfile_fwrite($fp3, "hello world....\n", 9999));

echo rockfile_fread($fp3, 8) . "\n";
echo rockfile_fread($fp3, 999) . "\n";

rockfile_fclose($fp1);  //关闭文件句柄, $fp2也会被关闭，显示关闭了文件句柄就不能被下一次请求复用，而未关闭的 $fp3 则可以被下一个请求复用，不需要重新打开文件

```
## 性能测试
### 原生PHP
···php
<?php
var_dump(file_put_contents("test0.txt", "少小离家老大回，乡音无改鬓毛衰。儿童相见不相识，笑问客从何处来。\n", FILE_APPEND));
var_dump(file_put_contents("test1.txt", "少小离家老大回，乡音无改鬓毛衰。儿童相见不相识，笑问客从何处来。\n", FILE_APPEND));
var_dump(file_put_contents("test2.txt", "少小离家老大回，乡音无改鬓毛衰。儿童相见不相识，笑问客从何处来。\n", FILE_APPEND));
var_dump(file_put_contents("test3.txt", "少小离家老大回，乡音无改鬓毛衰。儿童相见不相识，笑问客从何处来。\n", FILE_APPEND));
var_dump(file_put_contents("test4.txt", "少小离家老大回，乡音无改鬓毛衰。儿童相见不相识，笑问客从何处来。\n", FILE_APPEND));
var_dump(file_put_contents("test5.txt", "少小离家老大回，乡音无改鬓毛衰。儿童相见不相识，笑问客从何处来。\n", FILE_APPEND));
var_dump(file_put_contents("test6.txt", "少小离家老大回，乡音无改鬓毛衰。儿童相见不相识，笑问客从何处来。\n", FILE_APPEND));
var_dump(file_put_contents("test7.txt", "少小离家老大回，乡音无改鬓毛衰。儿童相见不相识，笑问客从何处来。\n", FILE_APPEND));
var_dump(file_put_contents("test8.txt", "少小离家老大回，乡音无改鬓毛衰。儿童相见不相识，笑问客从何处来。\n", FILE_APPEND));
var_dump(file_put_contents("test9.txt", "少小离家老大回，乡音无改鬓毛衰。儿童相见不相识，笑问客从何处来。\n", FILE_APPEND));

Concurrency Level:      100
Time taken for tests:   2.749 seconds
Complete requests:      10000
Failed requests:        0
Write errors:           0
Total transferred:      2430000 bytes
HTML transferred:       800000 bytes
Requests per second:    3637.36 [#/sec] (mean)
Time per request:       27.492 [ms] (mean)
Time per request:       0.275 [ms] (mean, across all concurrent requests)
Transfer rate:          863.16 [Kbytes/sec] received
···

### rockfile
```
<?php
$fp = rockfile_fopen("test2.txt", "ab");

var_dump(rockfile_fwrite($fp, "少小离家老大回，乡音无改鬓毛衰。儿童相见不相识，笑问客从何处来。\n"));

//rockfile_fclose($fp);  //不关闭文件句柄可以被别的请求复用

Concurrency Level:      100
Time taken for tests:   2.183 seconds
Complete requests:      10000
Failed requests:        1
   (Connect: 0, Receive: 0, Length: 1, Exceptions: 0)
Write errors:           0
Total transferred:      2430130 bytes
HTML transferred:       800130 bytes
Requests per second:    4580.67 [#/sec] (mean)
Time per request:       21.831 [ms] (mean)
Time per request:       0.218 [ms] (mean, across all concurrent requests)
Transfer rate:          1087.07 [Kbytes/sec] received

```


