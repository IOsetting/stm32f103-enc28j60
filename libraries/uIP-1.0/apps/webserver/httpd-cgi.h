#ifndef __HTTPD_CGI_H__
#define __HTTPD_CGI_H__


#include "httpd.h"

typedef PT_THREAD((* httpd_cgifunction)(struct httpd_state *, char *));

httpd_cgifunction httpd_cgi(char *name);

struct httpd_cgi_call {
  const char *name;
  const httpd_cgifunction function;
};

/**
 * \brief      HTTPD CGI function declaration
 * \param name The C variable name of the function
 * \param str  The string name of the function, used in the script file
 * \param function A pointer to the function that implements it
 *
 *             This macro is used for declaring a HTTPD CGI
 *             function. This function is then added to the list of
 *             HTTPD CGI functions with the httpd_cgi_add() function.
 *
 * \hideinitializer
 */
#define HTTPD_CGI_CALL(name, str, function) \
static PT_THREAD(function(struct httpd_state *, char *)); \
static const struct httpd_cgi_call name = {str, function}

void httpd_cgi_init(void);
#endif /* __HTTPD_CGI_H__ */

/** @} */
