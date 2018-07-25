/*
 * Routines for making a HTTP connection to a server on the Internet,
 * sending a simple HTTP GET request, and interpreting the response headers
 * that come back.
 *
 * E. Stark, 11/18/97 for CSE 230
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/socket.h>
#include <assert.h>

#include "url.h"
#include "http.h"
// #include "debug.h"

typedef struct HDRNODE *HEADERS;
HEADERS http_parse_headers(HTTP *http);
void http_free_headers(HEADERS head);
char *tempKeyParam;

/*
 * Routines to manage HTTP connections
 */
typedef enum { ST_REQ, ST_HDRS, ST_BODY, ST_DONE } HTTP_STATE;

struct http {
  FILE *file;             /* Stream to remote server */
  HTTP_STATE state;       /* State of the connection */
  int code;               /* Response code */
  char version[4];        /* HTTP version from the response */
  char *response;         /* Response string with message */
  HEADERS headers;        /* Reply headers */
};

/*
 * Open an HTTP connection for a specified IP address and port number
 */
HTTP * http_open(IPADDR *addr, int port) {
  HTTP *http = NULL;                  // initialized all below declared -F
  struct sockaddr_in sa = {0};
  int sock = 0;
  // Combined -F
  if(addr == NULL || (http = malloc(sizeof(*http))) == NULL)
    return(NULL);

  bzero(http, sizeof(*http));
  if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    free(http);
    return(NULL);
  }

  bzero(&sa, sizeof(sa));
  // sa.sin_len = sizeof(sa);             // In other file -F
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);
  bcopy(addr, &sa.sin_addr.s_addr, sizeof(struct in_addr));
  if(connect(sock, (struct sockaddr *)(&sa), sizeof(sa)) < 0 || (http->file = fdopen(sock, "w+")) == NULL) {
    free(http);
    close(sock);
    return(NULL);
  }

  http->state = ST_REQ;
  return(http);
}

/*
 * Close an HTTP connection that was previously opened.
 */

int http_close(HTTP *http) {
  if(http == NULL)            // Added -F
    return EOF;

  int err;
  err = fclose(http->file);
  free(http->response);
  http_free_headers(http->headers);
  free(http);
  return(err);
}

/*
 * Obtain the underlying FILE in an HTTP connection.
 * This can be used to issue additional headers after the request.
 */

FILE * http_file(HTTP *http) {
  if(http == NULL)              // Added -F
    return NULL;
  return(http->file);
}

/*
 * Issue an HTTP GET request for a URL on a connection.
 * After calling this function, the caller may send additional
 * headers, if desired, then must call http_response() before
 * attempting to read any document returned on the connection.
 */
int http_request(HTTP *http, URL *up) {
  // Added -F
  if(http == NULL || up == NULL || http->state != ST_REQ)
    return 1;

  void *prev = NULL;            // initialize -F

  /* Ignore SIGPIPE so we don't die while doing this */
  prev = signal(SIGPIPE, SIG_IGN);
  if(fprintf(http->file, "GET %s://%s:%d%s HTTP/1.0\r\nHost: %s\r\n",
   url_method(up), url_hostname(up), url_port(up),
   url_path(up), url_hostname(up)) == -1) {
       signal(SIGPIPE, prev);
       return(1);
  }

  http->state = ST_HDRS;
  signal(SIGPIPE, prev);
  return(0);
}

/*
 * Finish outputting an HTTP request and read the reply
 * headers from the response.  After calling this, http_getc()
 * may be used to collect any document returned as part of the
 * response.
 */

int http_response(HTTP *http) {
  // Added -F
  if(http == NULL || http->state != ST_HDRS)
    return(1);

  // initialze -F
  void *prev = NULL;
  char *response = NULL;
  size_t len = 0;

  /* Ignore SIGPIPE so we don't die while doing this */
  prev = signal(SIGPIPE, SIG_IGN);
  if(fprintf(http->file, "\r\n") == -1 || fflush(http->file) == EOF) {
    signal(SIGPIPE, prev);
    return(1);
  }

  rewind(http->file);
  signal(SIGPIPE, prev);
  len = getline(&response, &len, http->file);

  // +1 for null terminal
  if(response == NULL || (http->response = malloc(len+1)) == NULL) {
    return(1);
  }
  // Because len is type of size_t, you need to cast it. And to ensure no error, make sure it is not -1
  if((int)len == -1){
    free(response);
    return 1;
  }

  strncpy(http->response, response, len);

  do {
    http->response[len--] = '\0';
  } while((int)len >= 0 && (http->response[len] == '\r' || http->response[len] == '\n'));

  if(sscanf(http->response, "HTTP/%3s %d ", http->version, &http->code) != 2) {
    // free(http->response);        // I is add I is comment -F
    free(response);
    return(1);
  }

  http->headers = http_parse_headers(http);
  http->state = ST_BODY;
  // free(http->response);            // I is add I is comment -F
  // http_close(http);                // I is add I is comment -F
  free(response);                     // I is add -F
  return(0);
}

/*
 * Retrieve the HTTP status line and code returned as the
 * first line of the response from the server
 */

char * http_status(HTTP *http, int *code) {
  // Added the first arg
  if(http == NULL || http->state != ST_BODY)
    return(NULL);
  if(code != NULL)
    *code = http->code;
  return(http->response);
}

/*
 * Read the next character of a document from an HTTP connection
 */

int http_getc(HTTP *http) {
  // Added the first arg
  if(http == NULL || http->state != ST_BODY)
    return(EOF);
  return(fgetc(http->file));
}

/*
 * Routines for parsing the RFC822-style headers that come back
 * as part of the response to an HTTP request.
 */

typedef struct HDRNODE {
  char *key;
  char *value;
  struct HDRNODE *next;
} HDRNODE;

/*
 * Function for parsing RFC 822 header lines directly from input stream.
 */
HEADERS http_parse_headers(HTTP *http) {
  if(http == NULL)            // added -F
    return NULL;

  FILE *f = http->file;         // initialize -F
  HEADERS head = NULL, last = NULL;
  HDRNODE *node;
  size_t len = 0;
  ssize_t nread = 0;
  char *line, *l, *ll, *cp;
  line = l = ll = cp = NULL;

  // modified -F changed from fgetln to getline
  while((nread = getline(&ll, &len, f)) != -1) {
    len = nread;                // added -F
    line = l = malloc(len+1);
    l[len] = '\0';
    strncpy(l, ll, len);

    while((int)len > 0 && (l[len-1] == '\n' || l[len-1] == '\r'))
     l[--len] = '\0';

    if(len == 0) {
      free(line);
      break;
    }

    node = malloc(sizeof(HDRNODE));
    node->next = NULL;

    // added -F
    if(last == NULL)
      head = node;
    else
      last->next = node;

    for(cp = l; *cp == ' '; cp++);
    l = cp;

    for( ; *cp != ':' && *cp != '\0'; cp++);

    if(*cp == '\0' || *(cp+1) != ' ') {
      free(line);
      free(node);
      continue;
    }

    *cp++ = '\0';
    node->key = strdup(l);
    // printf("###\t%s\n", node->key);

    while(*cp == ' ')
      cp++;

    node->value = strdup(cp);
    for(cp = node->key; *cp != '\0'; cp++) {
      // if(isupper(*cp))
      //  *cp = tolower(*cp);
    }

    last = node;
    node = node->next;

    free(line);
    // free(cp);            // I is add. I is comment -F
    // free(l);
    // free(ll);
  }

  free(ll);                 // I is add here -F
  return(head);
}

/*
 * Free headers previously created by http_parse_headers()
 */

void http_free_headers(HEADERS head) {
  HEADERS next = NULL;      // I initialize here
  while(head != NULL) {
    // printf("%s\n", head->key);
    free(head->key);
    free(head->value);
    next = head->next;
    free(head);
    head = next;
  }
}

/*
 * Find the value corresponding to a given key in the headers
 */
char * http_headers_lookup(HTTP *http, char *key) {
  tempKeyParam = NULL;
  if(http == NULL || key == NULL)       // Added -F
    return(NULL);
  HEADERS head = http->headers;
  while(head != NULL) {
    if(!strcasecmp(head->key, key)) {
      tempKeyParam = head->key;
       return(head->value);
     }
    head = head->next;
  }
  return(NULL);
}