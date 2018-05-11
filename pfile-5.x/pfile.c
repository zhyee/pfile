/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
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
#include "php_pfile.h"

#define PFILE_RESOURCE_NAME "pfile handler"

/* If you declare any globals in php_pfile.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(pfile)
*/

/* True global resources - no need for thread safety here */
static int le_pfile;

/* {{{ pfile_functions[]
 *
 * Every user visible function must have an entry in pfile_functions[].
 */
const zend_function_entry pfile_functions[] = {
	//PHP_FE(confirm_pfile_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(pfile_fopen, NULL)
	PHP_FE(pfile_fwrite, NULL)
	PHP_FE(pfile_fclose, NULL)
	PHP_FE_END	/* Must be the last line in pfile_functions[] */
};
/* }}} */

/* {{{ pfile_module_entry
 */
zend_module_entry pfile_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"pfile",
	pfile_functions,
	PHP_MINIT(pfile),
	PHP_MSHUTDOWN(pfile),
	PHP_RINIT(pfile),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(pfile),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(pfile),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PFILE
ZEND_GET_MODULE(pfile)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("pfile.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_pfile_globals, pfile_globals)
    STD_PHP_INI_ENTRY("pfile.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_pfile_globals, pfile_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_pfile_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_pfile_init_globals(zend_pfile_globals *pfile_globals)
{
	pfile_globals->global_value = 0;
	pfile_globals->global_string = NULL;
}
*/
/* }}} */


static void pfile_regular_handler_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC){
	php_printf("资源释放\n");
}

static void pfile_persistent_handler_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
	php_printf("持久资源释放\n");
    pfile_handler *handler = (pfile_handler *)rsrc->ptr;
    fclose(handler->fp);
    pefree(handler->handler_name, 1);
    pefree(handler, 1);
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(pfile)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	// 注册资源
	le_pfile = zend_register_list_destructors_ex(pfile_regular_handler_dtor, pfile_persistent_handler_dtor, PFILE_RESOURCE_NAME, module_number);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(pfile)
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
PHP_RINIT_FUNCTION(pfile)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(pfile)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(pfile)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "pfile support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_pfile_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_pfile_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "pfile", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/

PHP_FUNCTION(pfile_fopen) {
	list_entry *lep;
	char *filepath, *mode, *handler_name;
	int pathlen, modelen, handler_name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &filepath, &pathlen, &mode, &modelen) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "incorrect parameters");
		RETURN_FALSE;
	}

	handler_name_len = spprintf(&handler_name, 0, "pfile*%s*%s", filepath, mode);

	if (zend_hash_find(&EG(persistent_list), handler_name, handler_name_len + 1, &lep) == SUCCESS) {
		if (lep->type == le_pfile){
			ZEND_REGISTER_RESOURCE(return_value, lep->ptr, lep->type);
			efree(handler_name);
			return;
		}
	}

	pfile_handler *handler;
	FILE *fp;
	list_entry le;

	fp = fopen(filepath, mode);
	php_printf("打开文件\n");

	if (fp == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable open file %s in mode %s", filepath, mode);
		RETURN_FALSE;
	}

	handler = pemalloc(sizeof(handler), 1);
	handler->handler_name = pemalloc(handler_name_len + 1, 1);
	memcpy(handler->handler_name, handler_name, handler_name_len);
	handler->handler_name[handler_name_len] = '\0';
	efree(handler_name);

	handler->fp = fp;

	le.type = le_pfile;
	le.ptr = handler;
	le.refcount = 0;
	ZEND_REGISTER_RESOURCE(return_value, handler, le_pfile);
	zend_hash_update(&EG(persistent_list), handler->handler_name, handler_name_len + 1, &le, sizeof(list_entry), NULL);
}

PHP_FUNCTION(pfile_fwrite){
	zval *file_resource;
	pfile_handler *handler;
	char *data;
	int datalen;
	long length = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|l", &file_resource, &data, &datalen, &length) == FAILURE){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "incorrect parameters");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(handler, pfile_handler *, &file_resource, -1, PFILE_RESOURCE_NAME, le_pfile);

	if (length <= 0 || length > (long)datalen) {
		length = (long)datalen;
	}

	RETURN_LONG(fwrite(data, 1, length, handler->fp));
}

// 关闭资源
PHP_FUNCTION(pfile_fclose){
	zval *file_resource;
	zend_bool full_close = 0;
	pfile_handler *handler;
	list_entry *lep;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|b", &file_resource, &full_close) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "incorrect parameters");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(handler, pfile_handler *, &file_resource, -1, PFILE_RESOURCE_NAME, le_pfile);
	zend_hash_index_del(&EG(regular_list), Z_RESVAL_P(file_resource));

	if (full_close){
		zend_hash_del(&EG(persistent_list), handler->handler_name, strlen(handler->handler_name) + 1);
	}
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
