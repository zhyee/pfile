/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
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

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("pfile.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_pfile_globals, pfile_globals)
    STD_PHP_INI_ENTRY("pfile.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_pfile_globals, pfile_globals)
PHP_INI_END()
*/
/* }}} */

ZEND_RSRC_DTOR_FUNC(pfile_regular_handler_dtor){
	php_printf("资源释放\n");
}

ZEND_RSRC_DTOR_FUNC(pfile_persistent_handler_dtor){
	php_printf("持久资源释放\n");
	if (res->ptr){
	    pfile_handler *handler = (pfile_handler *)res->ptr;
	    fclose(handler->fp);
	    pefree(handler->handler_name, 1);
	    pefree(handler, 1);
	    res->ptr = NULL;
	}
}

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_pfile_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_pfile_compiled)
{
	char *arg = NULL;
	size_t arg_len, len;
	zend_string *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	strg = strpprintf(0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "pfile", arg);

	RETURN_STR(strg);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and
   unfold functions in source code. See the corresponding marks just before
   function definition, where the functions purpose is also documented. Please
   follow this convention for the convenience of others editing your code.
*/




PHP_FUNCTION(pfile_fopen) {
	zend_resource *leptr;
	zend_string *handler_name;
	char *filepath, *mode;
	int pathlen, modelen;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &filepath, &pathlen, &mode, &modelen) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "incorrect parameters");
		RETURN_FALSE;
	}

	handler_name = strpprintf(0, "pfile*%s*%s", SAFE_STR(filepath), SAFE_STR(mode));

	if ((leptr = zend_hash_find_ptr(&EG(persistent_list), handler_name)) != NULL) {
		if (leptr->type == le_pfile){
			ZEND_REGISTER_RESOURCE(return_value, leptr->ptr, leptr->type);
			return;
		}
	}

	pfile_handler *handler;
	FILE *fp;
	zend_resource le;

	fp = fopen(filepath, mode);
	php_printf("打开文件\n");

	if (fp == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable open file %s in mode %s", filepath, mode);
		RETURN_FALSE;
	}

	handler = pemalloc(sizeof(handler), 1);
	handler->handler_name = pemalloc(sizeof(zend_string), 1);
	memcpy(handler->handler_name, handler_name, sizeof(zend_string));

	handler->fp = fp;

	le.type = le_pfile;
	le.ptr = handler;
	ZEND_REGISTER_RESOURCE(return_value, handler, le_pfile);
	zend_hash_str_update_mem(&EG(persistent_list), ZSTR_VAL(handler_name), ZSTR_LEN(handler_name), &le, sizeof(le));
}


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

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(pfile)
{
	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/
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
#if defined(COMPILE_DL_PFILE) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
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

/* {{{ pfile_functions[]
 *
 * Every user visible function must have an entry in pfile_functions[].
 */
const zend_function_entry pfile_functions[] = {
	PHP_FE(confirm_pfile_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(pfile_fopen, NULL)
	PHP_FE_END	/* Must be the last line in pfile_functions[] */
};
/* }}} */

/* {{{ pfile_module_entry
 */
zend_module_entry pfile_module_entry = {
	STANDARD_MODULE_HEADER,
	"pfile",
	pfile_functions,
	PHP_MINIT(pfile),
	PHP_MSHUTDOWN(pfile),
	PHP_RINIT(pfile),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(pfile),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(pfile),
	PHP_PFILE_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PFILE
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(pfile)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
