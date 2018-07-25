#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "snarf.h"

int opterr;
int optopt;
int optind;
char *optarg;
char *url_to_snarf;
char *output_file;
char *keyParam;                               // Added -F


/**
* @brief Adds 2 strings together with a space inbetween
*/
char* stringConcat(char* word1, char* word2) {
    int length1, length2 = 0;
    for(length1 = 0; word1[length1] != '\0'; length1++);

    // Separate the words that are added
    word1[length1++] = ' ';

    while(word2[length2]!='\0')
      word1[length1++] = word2[length2++];

    // Add in null terminator at the end.
    word1[length1] = '\0';
    // printf("#####%s#####\n", word1);
    return word1;
}

/**
 * @brief Checks to make sure 2 strings are equal.
 *
 * @param word1 is the address of the first word
 * @param word2 is the address of the second word
 *
 * @return  1 if strings are equal
 *           0 if strings are not equal
 */
int stringEquals(char* word1, char* word2) {
    // End of string. String are equal
    if(*word1 == '\0' && *word2 == '\0')
        return 1;
    // Make sure the current char of both strings are equal
    else if(*word1 == *word2)
        return stringEquals(++word1, ++word2);
    // Strings are not equal.
    else
        return 0;
}

/**
* Takes the argument.
* Goes by index. The argument which is not a flag will be put as the url
*/
void parse_args(int argc, char *argv[]) {
  char option;
  keyParam = NULL;                            // initialize -F
  output_file = NULL;

  // Added in -F
  // Checks for -h and returns 0 if -h is in the arguments
  for (int i = 0; i < argc; i++) {       // declared int here -F
    if (stringEquals(argv[i], "-h")) {
        USAGE(argv[0]);
        exit(0);
    }
  }

  for (int i = 0; optind < argc; i++) {       // declared int here -F
    debug("%d opterr: %d", i, opterr);
    debug("%d optind: %d", i, optind);
    debug("%d optopt: %d", i, optopt);
    debug("optarg: %s", optarg);
    debug("%d argv[optind]: %s", i, argv[optind]);

    if ((option = getopt(argc, argv, "+q:o:")) != -1) {
      debug("option: %c", option);
      switch (option) {
        case 'q':
          info("Query header: %s", optarg);

          if(optarg != NULL && (stringEquals(optarg, "-o") || stringEquals(optarg, "-q"))) {
            USAGE(argv[0]);
            exit(-1);
          }

          // -o-o -q-o -o-q -otext.txt not valid
          if(optarg != NULL && *(optarg-1) != '\0') {
            debug("########%c\n", *(optarg-1));
            USAGE(argv[0]);
            exit(-1);
          }

          if(keyParam == NULL){               // Added -F
            keyParam = optarg;
          } else {
            keyParam = stringConcat(keyParam, optarg);
          }
          // keyParam = optarg;
          // printf("#####%s\n", keyParam);
          break;
        case 'o':
          info("Output file: %s", optarg);
          if(optarg != NULL && output_file != NULL) {
            USAGE(argv[0]);
            exit(-1);
          }

          // -o-o -q-o -o-q -otext.txt not valid
          if(optarg != NULL && *(optarg-1) != '\0') {
            debug("########%c\n", *(optarg-1));
            USAGE(argv[0]);
            exit(-1);
          }

          if(optarg != NULL && (stringEquals(optarg, "-o") || stringEquals(optarg, "-q"))) {
            USAGE(argv[0]);
            exit(-1);
          }
          output_file = optarg;
          break;
        case '?':
          if(optopt=='h') {      // -h is found
            USAGE(argv[0]);
            exit(0);
          } else if (optopt!='o' && optopt!='q'){
            fprintf(stderr, KRED "-%c is not a supported argument\n" KNRM, optopt);
          }                             // Modified -F

          USAGE(argv[0]);
          exit(-1);
          break;
        default:
          break;
      }
    } else if(argv[optind] != NULL) {
      info("URL to snarf: %s", argv[optind]);
      url_to_snarf = argv[optind];
      optind++;

      if(optind != argc) {
        USAGE(argv[0]);
        exit(-1);
      }
    }
  }
}