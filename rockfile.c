/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_rockfile.h"

#define ROCKFILE_RESOURCE_NAME "rockfile resource"

/* If you declare any globals in php_rockfile.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(rockfile)
*/

/* True global resources - no need for thread safety here */
static int le_rockfile;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("rockfile.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_rockfile_globals, rockfile_globals)
    STD_PHP_INI_ENTRY("rockfile.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_rockfile_globals, rockfile_globals)
PHP_INI_END()
*/
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_rockfile_compiled(string arg)
   Return a string to confirm that the module is compiled in */

PHP_FUNCTION(rockfile_fopen) {
    zend_rsrc_list_entry *leptr;
    char *filepath, *mode, *handler_name;
    int pathlen, modelen, handler_name_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &filepath, &pathlen, &mode, &modelen) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "incorrect parameters");
        RETURN_FALSE;
    }

    handler_name_len = spprintf(&handler_name, 0, ".*?ROCKFILE*%s*%s", filepath, mode);

    if (zend_hash_find(&EG(persistent_list), handler_name, handler_name_len + 1, &leptr) == SUCCESS) {
        if (leptr->type == le_rockfile){
            RETVAL_STRING(handler_name, 1);
            efree(handler_name);
            return;
        }
    }

    FILE *fp;
    zend_rsrc_list_entry le;
    
    fp = fopen(filepath, mode);

    if (fp == NULL) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable open file %s in mode %s", filepath, mode);
        RETURN_FALSE;
    }

    le.type = le_rockfile;
    le.ptr = fp;

    zend_hash_update(&EG(persistent_list), handler_name, handler_name_len + 1, &le, sizeof(zend_rsrc_list_entry), NULL);
    RETVAL_STRING(handler_name, 1);
    efree(handler_name);
}

//fread
PHP_FUNCTION(rockfile_fread){
    zval *file_handler;
    zend_rsrc_list_entry *leptr;
    long length;
    size_t readlen;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zl", &file_handler, &length) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "incorrect parameters");
        RETURN_FALSE;
    }

    if (length <= 0) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "length must be bigger than zero");
        RETURN_FALSE;
    }

    if (Z_TYPE_P(file_handler) != IS_STRING) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "file handler is invalid");
        RETURN_FALSE;       
    } 

    if (zend_hash_find(&EG(persistent_list), Z_STRVAL_P(file_handler), Z_STRLEN_P(file_handler) + 1, &leptr) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "file handler is invalid");
        RETURN_FALSE;
    }

    if (leptr->type != le_rockfile){
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "file handler is invalid");
        RETURN_FALSE;
    }
    
    FILE *fp = (FILE *)leptr->ptr;
    char *buf = emalloc((size_t)length);
    readlen = fread(buf, 1, (size_t)length, fp);
    RETVAL_STRINGL(buf, readlen, 1);
    efree(buf);
    return;
}


// fwrite
PHP_FUNCTION(rockfile_fwrite){
    zval *file_handler;
    zend_rsrc_list_entry *leptr;
    char *data;
    int datalen;
    long length = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zs|l", &file_handler, &data, &datalen, &length) == FAILURE){
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "incorrect parameters");
        RETURN_FALSE;
    }
    
    if (length <= 0 || length > (long)datalen) {
        length = (long)datalen;
    }

    if (Z_TYPE_P(file_handler) != IS_STRING) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "file handler is invalid");
        RETURN_FALSE;       
    } 

    if (zend_hash_find(&EG(persistent_list), Z_STRVAL_P(file_handler), Z_STRLEN_P(file_handler) + 1, &leptr) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "file handler is invalid");
        RETURN_FALSE;
    }

    if (leptr->type != le_rockfile){
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "file handler is invalid");
        RETURN_FALSE;
    }
 
    FILE *fp = (FILE *)leptr->ptr;
    RETURN_LONG(fwrite(data, 1, length, fp));
}

// 关闭资源
PHP_FUNCTION(rockfile_fclose){
    zval *file_handler;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &file_handler) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "incorrect parameters");
        RETURN_FALSE;
    }
    if (Z_TYPE_P(file_handler) != IS_STRING) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "file handler is invalid");
        RETURN_FALSE;       
    } 
 
    zend_hash_del(&EG(persistent_list), Z_STRVAL_P(file_handler), Z_STRLEN_P(file_handler) + 1);
    RETURN_TRUE;
}



/*
PHP_FUNCTION(confirm_rockfile_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "rockfile", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/* {{{ php_rockfile_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_rockfile_init_globals(zend_rockfile_globals *rockfile_globals)
{
	rockfile_globals->global_value = 0;
	rockfile_globals->global_string = NULL;
}
*/
/* }}} */

static void rockfile_persistent_handler_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
    //php_printf("持久资源释放\n");
    FILE *fp = (FILE *)rsrc->ptr;
    fclose(fp);
}



/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(rockfile)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
    le_rockfile = zend_register_list_destructors_ex(NULL, rockfile_persistent_handler_dtor, ROCKFILE_RESOURCE_NAME, module_number);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(rockfile)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(rockfile)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(rockfile)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(rockfile)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "rockfile support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ rockfile_functions[]
 *
 * Every user visible function must have an entry in rockfile_functions[].
 */
const zend_function_entry rockfile_functions[] = {
	//PHP_FE(confirm_rockfile_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(rockfile_fopen, NULL)
    PHP_FE(rockfile_fread, NULL)
    PHP_FE(rockfile_fwrite, NULL)
    PHP_FE(rockfile_fclose, NULL)

	PHP_FE_END	/* Must be the last line in rockfile_functions[] */
};
/* }}} */

/* {{{ rockfile_module_entry
 */
zend_module_entry rockfile_module_entry = {
	STANDARD_MODULE_HEADER,
	"rockfile",
	rockfile_functions,
	PHP_MINIT(rockfile),
	PHP_MSHUTDOWN(rockfile),
	PHP_RINIT(rockfile),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(rockfile),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(rockfile),
	PHP_ROCKFILE_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_ROCKFILE
ZEND_GET_MODULE(rockfile)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
