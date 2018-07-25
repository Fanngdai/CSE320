/**
* piazza 426
* What am I printing for the header?
* What is the format?
* Is the keyword supposed to be in lower case or whatever is in the doc?
* No whatever the doc is. No lower-case. If http_parse_headers function.
* If you comment the tolowercase out,
* and the code works then, comment it out
*
* \r\n OR \n
* stark said \n to 383               <- Bae told me to do it with \n
* and showed me this post before he saw @406 Where stark said yes
* witness David & this one indian girl also doing CSE320.
* this is baes last result ^ use \n
*
* stark said yes to \r\n on @406    <- Bae said to do this in the end. After debugging his code
* A certain 320 TA under Stark told me not to add any extra values to header after printing
* \n on 421 <- bae said you will not see anymore null termination on the terminal
*
*
* how about the last \r\n? Do I need it?
* Bae said \n
* would -q "" work?
* Yes because this is additional
*
* what if there are two -o values?
* Reject piazza post 383 - Stark answered
*
*/

/*
 * Example of the use of the HTTP package for "Internet Bumbler"
 * E. Stark, 11/18/97 for CSE 230
 *
 * This program takes a single argument, which is interpreted as the
 * URL of a document to be retrieved from the Web.  It uses the HTTP
 * package to parse the URL, make an HTTP connection to the remote
 * server, retrieve the document, and display the result code and string,
 * the response headers, and the data portion of the document.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "debug.h"
#include "http.h"
#include "url.h"
#include "snarf.h"

int main(int argc, char *argv[]) {
  URL *up;
  HTTP *http;
  IPADDR *addr;
  int port, c, code;
  port = c = 0;
  code = -1;
  char *status, *method;
  status = method = NULL;
  output_file = NULL;
  tempKeyParam = NULL;
  // (void) http;
  // (void) addr;
  // (void) port;
  // (void) c;
  // (void) code;
  // (void) status;
  // (void) method;

  parse_args(argc, argv);

  if(url_to_snarf == NULL) {
    fprintf(stderr, "Illegal URL: '%s'\n", argv[1]);
    exit(-1);
  } else if((up = url_parse(url_to_snarf)) == NULL) {
    fprintf(stderr, "Illegal URL: '%s'\n", argv[1]);
    exit(-1);
  }

  method = url_method(up);
  // printf("##################%s\n", method);
  addr = url_address(up);
  port = url_port(up);
  if(method == NULL || strcasecmp(method, "http")) {
    fprintf(stderr, "Only HTTP access method is supported\n");
    url_free(up);
    exit(-1);
  }

  if((http = http_open(addr, port)) == NULL) {
    fprintf(stderr, "Unable to contact host '%s', port %d\n",
      url_hostname(up) != NULL ? url_hostname(up) : "(NULL)", port);;
    url_free(up);
    exit(-1);
  }

  http_request(http, up);
/*
 * Additional RFC822-style headers can be sent at this point,
 * if desired, by outputting to url_file(up).  For example:
 *
 *     fprintf(url_file(up), "If-modified-since: 10 Jul 1997\r\n");
 *
 * would activate "Conditional GET" on most HTTP servers.
 */
  http_response(http);
  /*
   * At this point, response status and headers are available for querying.
   *
   * Some of the possible HTTP response status codes are as follows:
   *
   *	200	Success -- document follows
   *	302	Document moved
   *	400	Request error
   *	401	Authentication required
   *	403	Access denied
   *	404	Document not found
   *	500	Server error
   *
   * You probably want to examine the "Content-type" header.
   * Two possibilities are:
   *    text/html	The body of the document is HTML code
   *	text/plain	The body of the document is plain ASCII text
   */
    status = http_status(http, &code);

#ifdef DEBUG
    debug("%s", status);
#else
    (void) status;
#endif

  /*
  * Look for a word if specified. other skip.
  * print date and keyparam
  */
  // if(keyParam != NULL) {
  //   char *token = strtok(keyParam, " ");
  //   while (token != NULL) {
  //     if(http_headers_lookup(http, token) != NULL)
  //       fprintf(stderr, "%s: %s", token, http_headers_lookup(http, token));
  //     token = strtok(NULL, " ");
  //   }
  // }
  int keyParamFound = 0;
  char *h = NULL;
  if(keyParam != NULL) {
    char *token = strtok(keyParam, " ");
    while (token != NULL) {
      if((h = http_headers_lookup(http, token)) != NULL && tempKeyParam != NULL) {
        fprintf(stderr, "%s: %s\r\n", tempKeyParam, h);
        keyParamFound = 1;
      }
      tempKeyParam = NULL;
      h = NULL;
      token = strtok(NULL, " ");
    }
    if(keyParamFound)
      fprintf(stderr, "\r\n");
  }

  /*
  * To make a file if specified.
  * Other wise, print it out
  */
  if (output_file != NULL) {
    FILE * fp;
    fp = fopen(output_file,"w");
    if(fp == NULL) {
      http_close(http);
      url_free(up);
      exit(-1);
    }
    while((c = http_getc(http)) != EOF)
      fprintf(fp,"%c", c);
    fclose(fp);
  } else {
    while((c = http_getc(http)) != EOF)
      putchar(c);
  }
  /*
   * At this point, we can retrieve the body of the document,
   * character by character, using http_getc()
   */
  http_close(http);
  url_free(up);

  if(code == 200)
    exit(0);
  else
    exit(code);
}
