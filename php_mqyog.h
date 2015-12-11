/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2007 The PHP Group                                |
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

/* $Id: header 226204 2007-01-01 19:32:10Z iliaa $ */

#ifndef PHP_MQYOG_H
#define PHP_MQYOG_H

extern zend_module_entry mqyog_module_entry;
#define phpext_mqyog_ptr &mqyog_module_entry

#ifdef PHP_WIN32
#define PHP_MQYOG_API __declspec(dllexport)
#else
#define PHP_MQYOG_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

typedef struct mqyog {
    char *hostname;
    int hostname_len;
    int response_code;
    int response_pos;
    char *response_body;
} mqyog_t;

#define MQYOG_RES_NAME "Httpsqs Connection"

int get_response(mqyog_t *mqyog, char *query, int query_len TSRMLS_DC);
int h_strcmp(const char *a, const char *b);
int get_response_code(char *response_body);
int get_responses(mqyog_t *mqyog,char *query, int query_len TSRMLS_DC);

PHP_MINIT_FUNCTION(mqyog);
PHP_MSHUTDOWN_FUNCTION(mqyog);
PHP_RINIT_FUNCTION(mqyog);
PHP_RSHUTDOWN_FUNCTION(mqyog);
PHP_MINFO_FUNCTION(mqyog);

PHP_FUNCTION(mqyog_connect);
PHP_FUNCTION(mqyog_get);
PHP_FUNCTION(mqyog_put);
PHP_FUNCTION(mqyog_status);
PHP_FUNCTION(mqyog_view);
PHP_FUNCTION(mqyog_reset);
PHP_FUNCTION(mqyog_maxqueue);
PHP_FUNCTION(mqyog_synctime);

PHP_METHOD(Httpsqs, __construct);

ZEND_BEGIN_MODULE_GLOBALS(mqyog)
long default_port;
char *default_charset;
char *default_host;
long timeout;
ZEND_END_MODULE_GLOBALS(mqyog)

/* In every utility function you add that needs to use variables 
   in php_mqyog_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as MQYOG_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define MQYOG_G(v) TSRMG(mqyog_globals_id, zend_mqyog_globals *, v)
#else
#define MQYOG_G(v) (mqyog_globals.v)
#endif

#endif	/* PHP_MQYOG_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
