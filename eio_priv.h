/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2011 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Ruslan Osmanov <osmanov@php.net>                             |
   +----------------------------------------------------------------------+
*/

#ifndef EIO_PRIV_H
#define EIO_PRIV_H

extern const zend_function_entry eio_functions[];

#define PHP_EIO_GRP_DESCRIPTOR_NAME "EIO Group Descriptor"
#define PHP_EIO_REQ_DESCRIPTOR_NAME "EIO Request Descriptor"

/* {{{ Macros */

#ifdef ZTS
#define TSRMLS_FETCH_FROM_CTX(ctx) void ***tsrm_ls = (void ***) ctx
#define TSRMLS_SET_CTX(ctx) ctx = (void ***) tsrm_ls
#else
#define TSRMLS_FETCH_FROM_CTX(ctx)
#define TSRMLS_SET_CTX(ctx)
#endif

#define EIO_RET_IF_NOT_CALLABLE(callback) \
    char *func_name; \
if (callback && Z_TYPE_P(callback) != IS_NULL) { \
    if (!zend_is_callable(callback, 0, &func_name TSRMLS_CC)) { \
	php_error_docref(NULL TSRMLS_CC, E_WARNING, \
		"'%s' is not a valid callback", func_name); \
	efree(func_name); \
	RETURN_FALSE; \
    } \
    efree(func_name); \
}

#ifdef EIO_DEBUG
# define PHP_EIO_RET_IF_FAILED(req, eio_func) \
    if (!req || req->result != 0) { \
	php_error_docref(NULL TSRMLS_CC, E_ERROR, \
#eio_func " failed: %s", strerror(req->errorno)); \
	RETURN_FALSE; \
    }
#else
# define PHP_EIO_RET_IF_FAILED(req, eio_func) \
    if (!req || req->result != 0) \
RETURN_FALSE;
#endif

#define PHP_EIO_RET_REQ_RESOURCE(req, eio_func) \
    PHP_EIO_RET_IF_FAILED(req, eio_func); \
ZEND_REGISTER_RESOURCE(return_value, req, le_eio_req);

#define PHP_EIO_INIT(pri, callback, data, eio_cb, req) \
    long pri = EIO_PRI_DEFAULT; \
zval *callback = NULL, *data = NULL; \
php_eio_cb_t *eio_cb; \
eio_req *req;

#ifdef EIO_DEBUG
#define EIO_CHECK_PATH_LEN(path, path_len) \
    if (strlen(path) != path_len) { \
	php_error_docref(NULL TSRMLS_CC, E_WARNING, \
		"failed calculating path length"); \
	RETURN_FALSE; \
    }
#else
#define EIO_CHECK_PATH_LEN(path, path_len) \
    if (strlen(path) != path_len) { \
	RETURN_FALSE; \
    }
#endif

#define EIO_REGISTER_LONG_EIO_CONSTANT(name) \
    REGISTER_LONG_CONSTANT(#name, name, \
	    CONST_CS | CONST_PERSISTENT);

#define EIO_REGISTER_LONG_CONSTANT(name, value) \
    REGISTER_LONG_CONSTANT(#name, value, \
	    CONST_CS | CONST_PERSISTENT);

#define EIO_CB_CUSTOM_IS_LOCKED(eio_cb) ((eio_cb) ? (eio_cb)->locked : 0)

/* }}} */

/* {{{ Types */
#ifdef ZTS
#define EIO_CB_T_COMMON \
    zval* func;		/* Userspace callback */ \
zval* arg;		/* Argument for the userspace callback */ \
/* Thread context; to get rid of calling TSRMLS_FETCH() which consumes \
 * considerable amount of resources */ \
void ***thread_ctx;															
#else
#define EIO_CB_T_COMMON \
    zval* func; \
zval* arg;								
#endif

typedef struct php_eio_cb_ {
    EIO_CB_T_COMMON;
} php_eio_cb_t;

typedef struct php_eio_cb_custom_ {
    EIO_CB_T_COMMON;
    zval* exec;
    zend_bool locked;
} php_eio_cb_custom_t;

#undef EIO_CB_T_COMMON
/* }}} */

#endif	/* EIO_PRIV_H */
/*
 * vim600: ft=h.c ci pi sts=4 sw=4 ts=8 fdm=marker 
 * vim<600: ci pi sts=4 sw=4 ts=8
 */
