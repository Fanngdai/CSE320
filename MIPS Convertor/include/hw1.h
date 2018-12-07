#ifndef HW_H
#define HW_H

// Size of the opcodeTable and the specialTable
#define NUM_ELEM 64

#include "const.h"
#include "instruction.h"

#endif

int bigEndian(unsigned int num);
int stringEqual(char* word1, char* word2);
int countCommas(char* word);
int countDollaSign(char* word);
int countSpaces(char* word);
int sameParenthesis(char* word);