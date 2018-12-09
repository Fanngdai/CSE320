#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "url.h"

/*
 * Routines to interpret and manage URL's
 */

struct url {
  char *stuff;      /* Working storage containing all parts */
  char *method;     /* The access method (http, ftp, etc.) */
  char *hostname;   /* The server host name */
  int port;     /* The TCP port to contact */
  char *path;     /* The path of the document on the server */
  int dnsdone;      /* Have we done DNS lookup yet? */
  struct in_addr addr;    /* IP address of the server */
};

// Added -F
void url_init(URL *up) {
  up->stuff = NULL;      /* Working storage containing all parts */
  up->method = NULL;     /* The access method (http, ftp, etc.) */
  up->hostname = NULL;   /* The server host name */
  up->port = 0;     /* The TCP port to contact */
  up->path = NULL;     /* The path of the document on the server */
  up->dnsdone = 0;      /* Have we done DNS lookup yet? */
}

/*
 * Parse a URL given as a string into its constituent parts,
 * and return it as a URL object.
 */

URL * url_parse(char *url) {
  URL *up = NULL;             // initialize -F
  char *cp, c;
  char *slash, *colon;
  cp = slash = colon = NULL;  // initialize -F

  // Added first arg -F
  if(url == NULL || (up = malloc(sizeof(*up))) == NULL)
    return(NULL);

  // Added -F
  url_init(up);

  /*
   * Make a copy of the argument that we can fiddle with
   */
  if((up->stuff = strdup(url)) == NULL) {
    free(up);
    return(NULL);
  }
  up->dnsdone = 0;
  bzero(&up->addr, sizeof(struct in_addr));
  /*
   * Now ready to parse the URL
   */
  cp = up->stuff;
  slash = strchr(cp, '/');
  colon = strchr(cp, ':');
  // Added slash checker sec arg -F
  if(colon != NULL && slash != NULL) {
    /*
     * If a colon occurs before any slashes, then we assume the portion
     * of the URL before the colon is the access method.
     */
    if(colon < slash) {
      *colon = '\0';
      // free(up->method);   // - removed bc no need for it
      up->method = strdup(cp);
      cp = colon+1;
      if(!strcasecmp(up->method, "http"))
        up->port = 80;
    }
    if(slash != NULL && *(slash+1) == '/') {
      /*
       * If there are two slashes, then we have a full, absolute URL,
       * and the following string, up to the next slash, colon or the end
       * of the URL, is the host name.
       */
      for(cp = slash+2; *cp != '\0' && *cp != ':' && *cp != '/'; cp++);
      c = *cp;
      *cp = '\0';
      // free(up->hostname);          // Removed -F
      up->hostname = strdup(slash+2);
      *cp = c;
      /*
       * If we found a ':', then we have to collect the port number
       */
      if(*cp == ':') {
        char *cp1;
        cp1 = ++cp;
        while(isdigit(*cp))
          cp++;
        c = *cp;
        *cp = '\0';
        up->port = atoi(cp1);
        *cp = c;
      }
    }
    // printf("***\t%s\n", up->hostname);
    if(*cp == '\0')
      up->path = "/";
    else
      up->path = cp;
  } else {
    /*
     * No colon: a relative URL with no method or hostname
     */
    up->path = cp;
  }
  return(up);
}

/*
 * Free a URL object that was previously created by url_parse()
 */
void url_free(URL *up) {
  // added the if -F
  if(up != NULL){
    free(up->stuff);
    if(up->method != NULL)
      free(up->method);
    if(up->hostname != NULL)
      free(up->hostname);
    free(up);
  }
}

/*
 * Extract the "access method" portion of a URL
 */

char * url_method(URL *up) {
  if(up == NULL)
    return NULL;
  return(up->method);
}

/*
 * Extract the hostname portion of a URL
 */

char * url_hostname(URL *up) {
  if(up == NULL)
    return NULL;
  return(up->hostname);
}

/*
 * Obtain the TCP port number portion of a URL
 */

int url_port(URL *up) {
  if(up == NULL)
    return EOF;
  return(up->port);
}

/*
 * Obtain the path portion of a URL
 */

char * url_path(URL *up) {
  if(up == NULL)
    return NULL;
  return(up->path);
}

/*
 * Obtain the network (IP) address of the host specified in a URL.
 * This will cause a DNS lookup if the address is not already cached
 * in the URL object.
 */

IPADDR * url_address(URL *up) {
  if(up == NULL)                  // Added -F
    return NULL;

  struct hostent *he = {0};

  if(!up->dnsdone) {
    if(up->hostname != NULL && *up->hostname != '\0') {
      if((he = gethostbyname(up->hostname)) == NULL)
        return(NULL);
      bcopy(he->h_addr, &up->addr, sizeof(struct in_addr));
    }
    up->dnsdone = 1;
  }
  return(&up->addr);
}
