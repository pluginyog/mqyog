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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_mqyog.h"

#define MQYOG_PUT_OK 			0
#define MQYOG_PUT_ERROR 		1
#define MQYOG_PUT_END 		2
#define MQYOG_GET_END 		3
#define MQYOG_RESET_OK 		4
#define MQYOG_RESET_ERROR 	5
#define MQYOG_MAXQUEUE_OK		6
#define MQYOG_MAXQUEUE_CANCEL 7
#define MQYOG_SYNCTIME_OK		8
#define MQYOG_SYNCTIME_CANCEL 9
#define MQYOG_ERROR			10

ZEND_DECLARE_MODULE_GLOBALS(mqyog)

/* True global resources - no need for thread safety here */
static int le_mqyog;
static zend_class_entry *mqyog_class_entry_ptr;

/* {{{ mqyog_functions[]
 *
 * Every user visible function must have an entry in mqyog_functions[].
 */
zend_function_entry mqyog_functions[] = {
		PHP_FE(mqyog_connect, NULL)
		PHP_FE(mqyog_get, NULL)
		PHP_FE(mqyog_put, NULL)
		PHP_FE(mqyog_status, NULL)
		PHP_FE(mqyog_view, NULL)
		PHP_FE(mqyog_reset, NULL)
		PHP_FE(mqyog_maxqueue, NULL)
		PHP_FE(mqyog_synctime, NULL)

		{NULL, NULL, NULL}	/* Must be the last line in mqyog_functions[] */
};
/* }}} */

static zend_function_entry mqyog_class_functions[] = {
		PHP_ME(Httpsqs, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
		PHP_FALIAS(get, mqyog_get, NULL)
		PHP_FALIAS(put, mqyog_put, NULL)
		PHP_FALIAS(status, mqyog_status, NULL)
		PHP_FALIAS(view, mqyog_view, NULL)
		PHP_FALIAS(reset, mqyog_reset, NULL)
		PHP_FALIAS(maxqueue, mqyog_maxqueue, NULL)
		PHP_FALIAS(synctime, mqyog_synctime, NULL)
		{NULL, NULL, NULL}
};

/* {{{ mqyog_module_entry
 */
zend_module_entry mqyog_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
		STANDARD_MODULE_HEADER,
#endif
		"mqyog",
		mqyog_functions,
		PHP_MINIT(mqyog),
		PHP_MSHUTDOWN(mqyog),
		PHP_RINIT(mqyog),		/* Replace with NULL if there's nothing to do at request start */
		PHP_RSHUTDOWN(mqyog),	/* Replace with NULL if there's nothing to do at request end */
		PHP_MINFO(mqyog),
#if ZEND_MODULE_API_NO >= 20010901
		"0.1", /* Replace with version number for your extension */
#endif
		STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_MQYOG
ZEND_GET_MODULE(mqyog)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("mqyog.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_mqyog_globals, mqyog_globals)
    STD_PHP_INI_ENTRY("mqyog.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_mqyog_globals, mqyog_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_mqyog_init_globals
 */
static void php_mqyog_init_globals(zend_mqyog_globals *mqyog_globals_p TSRMLS_DC)
{
	MQYOG_G(default_port) = 1218;
	MQYOG_G(default_charset) = "utf-8";
	MQYOG_G(default_host) = "127.0.0.1";
	MQYOG_G(timeout) = 20;
}
/* }}} */

static void php_mqyog_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
	mqyog_t *mqyog = (mqyog_t *)rsrc->ptr;
	efree(mqyog->response_body);
	efree(mqyog->hostname);
	efree(mqyog);
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(mqyog)
		{
				/* If you have INI entries, uncomment these lines 
                REGISTER_INI_ENTRIES();
                */
				zend_class_entry mqyog_class_entry;
		INIT_CLASS_ENTRY(mqyog_class_entry, "Httpsqs", mqyog_class_functions);
		mqyog_class_entry_ptr = zend_register_internal_class(&mqyog_class_entry TSRMLS_CC);

#ifdef ZTS
		ts_allocate_id(&mqyog_globals_id, sizeof(zend_mqyog_globals), (ts_allocate_ctor) php_mqyog_init_globals, NULL);
#else
		php_mqyog_init_globals(&mqyog_globals TSRMLS_CC);
#endif

		le_mqyog = zend_register_list_destructors_ex(php_mqyog_dtor, NULL, MQYOG_RES_NAME, module_number);

		return SUCCESS;
		}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(mqyog)
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
PHP_RINIT_FUNCTION(mqyog)
		{
				return SUCCESS;
		}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(mqyog)
		{
				return SUCCESS;
		}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(mqyog)
		{
				php_info_print_table_start();
		php_info_print_table_header(2, "mqyog support", "enabled");
		php_info_print_table_end();

				/* Remove comments if you have entries in php.ini
                DISPLAY_INI_ENTRIES();
                */
		}
/* }}} */

static void mqyog_connect(INTERNAL_FUNCTION_PARAMETERS) {
	zval *object = getThis();
	mqyog_t *mqyog;
	char *hostname, *host = MQYOG_G(default_host);
	long port = MQYOG_G(default_port);
	int hostname_len, host_len, list_id;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sl", &host, &host_len, &port) == FAILURE) {
		RETURN_NULL();
	}

	hostname_len = spprintf(&hostname, 0, "%s:%d", host, port);

	mqyog = emalloc(sizeof(mqyog_t));
	mqyog->hostname = estrndup(hostname, hostname_len);
	mqyog->hostname_len = hostname_len;
	efree(hostname);

	if (!object) {
		ZEND_REGISTER_RESOURCE(return_value, mqyog, le_mqyog);
	} else {
		list_id = zend_list_insert(mqyog, le_mqyog);
		add_property_resource(object, "connection", list_id);
	}

}

PHP_METHOD(Httpsqs, __construct) {
mqyog_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_FUNCTION(mqyog_connect) {
		mqyog_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_FUNCTION(mqyog_get) {
		mqyog_t *mqyog;
		zval *hr, **connection, *object = getThis();
		zend_bool return_array = 0;
		char *queuename, *charset = MQYOG_G(default_charset), *query;
		int queuename_len, charset_len, query_len;

		if (object) {
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|bs", &queuename, &queuename_len, &return_array, &charset, &charset_len) == FAILURE) {
				RETURN_NULL();
			}
			if (zend_hash_find(Z_OBJPROP_P(object), "connection", sizeof("connection"),(void **) &connection) == FAILURE) {
				RETURN_NULL();
			}
			MAKE_STD_ZVAL(hr);
			ZVAL_RESOURCE(hr, Z_LVAL_PP(connection));
		} else {
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|bs", &hr, &queuename, &queuename_len, &return_array, &charset, &charset_len) == FAILURE) {
				RETURN_NULL();
			}
		}

		ZEND_FETCH_RESOURCE(mqyog, mqyog_t*, &hr, -1, MQYOG_RES_NAME, le_mqyog);

		query_len = spprintf(&query, 0, "GET /?name=%s&opt=get&charset=%s HTTP/1.1\r\nConnection:close\r\n\r\n", queuename, charset);
		if (!get_response(mqyog, query, query_len TSRMLS_CC)) {
			RETURN_FALSE;
		}
		efree(query);

		if (mqyog->response_code == MQYOG_GET_END || mqyog->response_code == MQYOG_ERROR) {
			RETURN_FALSE;
		} else {
			if (!return_array) {
				RETURN_STRING(mqyog->response_body, 1);
			} else {
				zval *pos, *data;
				MAKE_STD_ZVAL(pos);
				MAKE_STD_ZVAL(data);

				ZVAL_LONG(pos, mqyog->response_pos);
				ZVAL_STRING(data, mqyog->response_body, 1);

				array_init(return_value);

				zend_hash_update(HASH_OF(return_value), "pos", sizeof("pos"), &pos, sizeof(zval*), NULL);
				zend_hash_update(HASH_OF(return_value), "data", sizeof("data"), &data, sizeof(zval*), NULL);
			}
		}
}
PHP_FUNCTION(mqyog_put) {
		mqyog_t *mqyog;
		zval *hr, **connection, *object = getThis();
		zend_bool return_array = 0;
		char *queuename,*data,  *charset = MQYOG_G(default_charset), *query;
		int queuename_len, data_len,charset_len, query_len;
		if (object) {
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s", &queuename, &queuename_len, &data, &data_len) == FAILURE) {
				RETURN_FALSE;
			}
			if (zend_hash_find(Z_OBJPROP_P(object), "connection", sizeof("connection"), (void **) &connection) == FAILURE) {
				RETURN_FALSE;
			}

			MAKE_STD_ZVAL(hr);
			ZVAL_RESOURCE(hr, Z_LVAL_PP(connection));
		} else {
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss|s", &hr, &queuename, &queuename_len, &data, &data_len) == FAILURE) {
				RETURN_FALSE;
			}
		}

		ZEND_FETCH_RESOURCE(mqyog, mqyog_t*, &hr, -1, MQYOG_RES_NAME, le_mqyog);

		query_len = spprintf(&query, 0, "GET /?name=%s&opt=put&data=%s&charset=utf-8 HTTP/1.1\r\nConnection:close\r\n\r\n", queuename, data);
		if (!get_response(mqyog, query, query_len TSRMLS_CC)) {
			RETURN_FALSE;
		}
		efree(query);

		if (mqyog->response_code == MQYOG_PUT_OK) {
			RETURN_TRUE;
		} else {
			RETURN_FALSE;
		}
}

PHP_FUNCTION(mqyog_status) {
		mqyog_t *mqyog;
		zval *hr, **connection, *object = getThis();
		zend_bool return_array = 0;
		char *queuename, *charset = MQYOG_G(default_charset), *query;
		int queuename_len, charset_len, query_len;

		if (object) {
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|bs", &queuename, &queuename_len, &return_array, &charset, &charset_len) == FAILURE) {
				RETURN_NULL();
			}
			if (zend_hash_find(Z_OBJPROP_P(object), "connection", sizeof("connection"),(void **) &connection) == FAILURE) {
				RETURN_NULL();
			}
			MAKE_STD_ZVAL(hr);
			ZVAL_RESOURCE(hr, Z_LVAL_PP(connection));
		} else {
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|bs", &hr, &queuename, &queuename_len, &return_array, &charset, &charset_len) == FAILURE) {
				RETURN_NULL();
		}
		}

		ZEND_FETCH_RESOURCE(mqyog, mqyog_t*, &hr, -1, MQYOG_RES_NAME, le_mqyog);
		query_len = spprintf(&query, 0, "GET /?name=%s&opt=status_json HTTP/1.1\r\nConnection:close\r\n\r\n", queuename);
		if (!get_responses(mqyog, query, query_len TSRMLS_CC)) {
			RETURN_FALSE;
		}
		 RETURN_STRING(mqyog->response_body, 1);


}

PHP_FUNCTION(mqyog_view) {
		mqyog_t *mqyog;
		zval *hr, **connection, *object = getThis();
		char *queuename, *query, *charset = MQYOG_G(default_charset);
		int queuename_len, query_len, charset_len, pos;

		if (object) {
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl|s", &queuename, &queuename_len, &pos, &charset, &charset_len) == FAILURE) {
				RETURN_FALSE;
			}
			if (zend_hash_find(Z_OBJPROP_P(object), "connection", sizeof("connection"), (void **) &connection) == FAILURE) {
				RETURN_FALSE;
			}

			MAKE_STD_ZVAL(hr);
			ZVAL_RESOURCE(hr, Z_LVAL_PP(connection));
		} else {
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsl|s", &hr, &queuename, &queuename_len, &pos, &charset, &charset_len) == FAILURE) {
				RETURN_NULL();
			}
		}

		ZEND_FETCH_RESOURCE(mqyog, mqyog_t*, &hr, -1, MQYOG_RES_NAME, le_mqyog);

		query_len = spprintf(&query, 0, "GET /?name=%s&opt=view&pos=%d&charset=%s HTTP/1.1\r\nConnection:close\r\n\r\n", queuename, pos, charset);
		if (!get_response(mqyog, query, query_len TSRMLS_CC)) {
			RETURN_FALSE;
		}
		efree(query);

		if (mqyog->response_code == MQYOG_ERROR) {
			RETURN_FALSE;
		} else {
			RETURN_STRING(mqyog->response_body, 1);
		}
}
PHP_FUNCTION(mqyog_reset) {
		mqyog_t *mqyog;
		zval *hr, **connection, *object = getThis();
		char *queuename, *query;
		int queuename_len, query_len;

		if (object) {
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &queuename, &queuename_len) == FAILURE) {
				RETURN_FALSE;
			}
			if (zend_hash_find(Z_OBJPROP_P(object), "connection", sizeof("connection"), (void **) &connection) == FAILURE) {
				RETURN_FALSE;
			}

			MAKE_STD_ZVAL(hr);
			ZVAL_RESOURCE(hr, Z_LVAL_PP(connection));
		} else {
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &hr, &queuename, &queuename_len) == FAILURE) {
				RETURN_NULL();
			}
		}

		ZEND_FETCH_RESOURCE(mqyog, mqyog_t*, &hr, -1, MQYOG_RES_NAME, le_mqyog);

		query_len = spprintf(&query, 0, "GET /?name=%s&opt=reset HTTP/1.1\r\nConnection:close\r\n\r\n", queuename);
		if (!get_response(mqyog, query, query_len TSRMLS_CC)) {
			RETURN_FALSE;
		}
		efree(query);


		if (mqyog->response_code == MQYOG_RESET_OK) {
			RETURN_TRUE;
		} else {
			RETURN_FALSE;
		}
}

PHP_FUNCTION(mqyog_maxqueue) {
		mqyog_t *mqyog;
		zval *hr, **connection, *object = getThis();
		char *queuename, *query;
		int queuename_len, query_len, maxqueue;

		if (object) {
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl|l", &queuename, &queuename_len, &maxqueue) == FAILURE) {
				RETURN_FALSE;
			}
			if (zend_hash_find(Z_OBJPROP_P(object), "connection", sizeof("connection"), (void **) &connection) == FAILURE) {
				RETURN_FALSE;
			}

			MAKE_STD_ZVAL(hr);
			ZVAL_RESOURCE(hr, Z_LVAL_PP(connection));
		} else {
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsl|l", &hr, &queuename, &queuename_len, &maxqueue) == FAILURE) {
				RETURN_NULL();
			}
		}

		ZEND_FETCH_RESOURCE(mqyog, mqyog_t*, &hr, -1, MQYOG_RES_NAME, le_mqyog);

		query_len = spprintf(&query, 0, "GET /?name=%s&opt=maxqueue&num=%d HTTP/1.1\r\nConnection:close\r\n\r\n", queuename, maxqueue);
		if (!get_response(mqyog, query, query_len TSRMLS_CC)) {
			RETURN_FALSE;
		}
		efree(query);

		if (mqyog->response_code == MQYOG_MAXQUEUE_OK) {
			RETURN_TRUE;
		} else {
			RETURN_FALSE;
		}

}

PHP_FUNCTION(mqyog_synctime) {
		mqyog_t *mqyog;
		zval *hr, **connection, *object = getThis();
		char *queuename, *query;
		int queuename_len, query_len, synctime;

		if (object) {
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &queuename, &queuename_len, &synctime) == FAILURE) {
				RETURN_FALSE;
			}
			if (zend_hash_find(Z_OBJPROP_P(object), "connection", sizeof("connection"), (void **) &connection) == FAILURE) {
				RETURN_FALSE;
			}

			MAKE_STD_ZVAL(hr);
			ZVAL_RESOURCE(hr, Z_LVAL_PP(connection));
		} else {
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsl", &hr, &queuename, &queuename_len, &synctime) == FAILURE) {
				RETURN_NULL();
			}
		}

		ZEND_FETCH_RESOURCE(mqyog, mqyog_t*, &hr, -1, MQYOG_RES_NAME, le_mqyog);

		query_len = spprintf(&query, 0, "GET /?name=%s&opt=synctime&num=%d HTTP/1.1\r\nConnection:close\r\n\r\n", queuename, synctime);
		if (!get_response(mqyog, query, query_len TSRMLS_CC)) {
			RETURN_FALSE;
		}
		efree(query);

		if (mqyog->response_code == MQYOG_SYNCTIME_OK) {
			RETURN_TRUE;
		} else {
			RETURN_FALSE;
		}
}

int get_response(mqyog_t *mqyog, char *query, int query_len TSRMLS_DC) {
	php_stream *stream;
	char *errstr = NULL;
	int errcode;
	struct timeval tv;

	tv.tv_sec = MQYOG_G(timeout);

	stream = php_stream_xport_create(mqyog->hostname, mqyog->hostname_len, ENFORCE_SAFE_MODE | REPORT_ERRORS, STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT, NULL, &tv, NULL, &errstr, &errcode);

	if (errstr) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "[%d]%s", errcode, errstr);
		efree(errstr);
		return 0;
	}

	php_stream_write(stream, query, query_len);

	char buf[4096], *response, *eptr;
	int response_len, pos = 0;
	int bodyflag = 0, posflag = 0;

	while (!stream->eof) {
		response = php_stream_get_line(stream, buf, 1024, &response_len);

		// 将响应中第一次出现的Pos:作为队列的位置 
		if (strspn(response, "Pos:") && posflag == 0) {
			int j = 0, k = 0;
			char num[10];
			while (response[j] != '\0') {
				if (isdigit(response[j])) {
					num[k] = response[j];
					k++;
				}
				j++;
			}
			num[k] = '\0';
			pos = strtol(num, &eptr, 10);
			posflag++;
		}

		// 说明这一行是body
		if (bodyflag == 1) {
			mqyog->response_body = estrndup(response, response_len);
		}

		// \r\n是响应head和body的分割符
		if (h_strcmp(response, "\r\n")) {
			bodyflag++;
		}

	}

	php_stream_close(stream);
	mqyog->response_pos = pos;
	mqyog->response_code = get_response_code(mqyog->response_body);

	return 1;
}


int h_strcmp(const char *a, const char *b) {
	if (strcmp(a, b) == 0) {
		return 1;
	}
	return 0;
}

int get_response_code(char *response) {
	if (h_strcmp(response, "MQ_ERROR")) {
		return MQYOG_ERROR;
	} else if (h_strcmp(response, "MQ_PUT_OK")) {
		return MQYOG_PUT_OK;
	} else if (h_strcmp(response, "MQ_PUT_END")) {
		return MQYOG_PUT_END;
	} else if (h_strcmp(response, "MQ_PUT_ERROR")) {
		return MQYOG_PUT_ERROR;
	} else if (h_strcmp(response, "MQ_GET_END")) {
		return MQYOG_GET_END;
	} else if (h_strcmp(response, "MQ_RESET_OK")) {
		return MQYOG_RESET_OK;
	} else if (h_strcmp(response, "MQ_RESET_ERROR")) {
		return MQYOG_RESET_ERROR;
	} else if (h_strcmp(response, "MQ_MAXQUEUE_OK")) {
		return MQYOG_MAXQUEUE_OK;
	} else if (h_strcmp(response, "MQ_MAXQUEUE_CANCEL")) {
		return MQYOG_MAXQUEUE_CANCEL;
	} else if (h_strcmp(response, "MQ_SYNCTIME_OK")) {
		return MQYOG_SYNCTIME_OK;
	} else if (h_strcmp(response, "MQ_SYNCTIME_CANCEL")) {
		return MQYOG_SYNCTIME_CANCEL;
	} else {
		return -1;
	}
}

int get_responses(mqyog_t *mqyog, char *query, int query_len TSRMLS_DC) {
	php_stream *stream;
	char *errstr = NULL;
	int errcode;
	struct timeval tv;

	tv.tv_sec = MQYOG_G(timeout);

	stream = php_stream_xport_create(mqyog->hostname, mqyog->hostname_len, ENFORCE_SAFE_MODE | REPORT_ERRORS, STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT, NULL, &tv, NULL, &errstr, &errcode);

	if (errstr) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "[%d]%s", errcode, errstr);
		efree(errstr);
		return 0;
	}

	php_stream_write(stream, query, query_len);

	char buf[4096], *response, *eptr;
	int response_len, pos = 0;
	int bodyflag = 0, posflag = 0;
	mqyog->response_body ="";
	while (!stream->eof) {
		response = php_stream_get_line(stream, buf, 1024, &response_len);
		mqyog->response_body = estrndup(response, response_len);
		
	}

	php_stream_close(stream);

	return 1;
}
