#include <stdlib.h>

// header gaurd.
#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

#include "hw1.h"
#include "debug.h"

int main(int argc, char **argv)
{
    debug("this is my argv[0]: %s", *argv);
    if(!validargs(argc, argv))
        USAGE(*argv, EXIT_FAILURE);
    debug("Options: 0x%X", global_options);
    if(global_options & 0x1)
        USAGE(*argv, EXIT_SUCCESS);

    char userInput[30];
    // Need to make an Instr to pass in as param for decode
    Instruction instr = {0};
    unsigned int addr = global_options & 0xFFFFF000;
    int endi = (global_options >> 2) & 0x1;

    // decode -d
    if((global_options >> 1) & 0x1) {
        // FILE *file;
        // char* fileName = 0;
        while(fread(&instr.value, sizeof(instr.value), 1, stdin)) {
            // Get instruction word from userInput and convert to int
            if(endi) {
                instr.value = bigEndian(instr.value);
            }

            if(decode(&instr, addr)) {
                printf(instr.info->format, *(instr.args), *(instr.args+1), *(instr.args+2));
                printf("\n");
                addr += 4;
            } else {
                return EXIT_FAILURE;
            }
        }
    }
    // encode -a
    else {
        int amtOfArg = 0;
        int countedArg = 0;

        while(fgets(userInput, sizeof(userInput), stdin)) {
            for(int i=0; i<NUM_ELEM+1; i++) {
                amtOfArg = (sscanf(userInput, (instrTable+i)->format, instr.args, instr.args+1, instr.args+2));
                if(i == NUM_ELEM) {  // Format not found
                    return EXIT_FAILURE;
                } else if( amtOfArg || stringEqual(userInput, (instrTable+i)->format)) {
                    instr.info = instrTable+i;
                    break;
                }
            }

            if((countCommas(userInput) >= amtOfArg && amtOfArg != 0)
                || countDollaSign(instr.info->format) != countDollaSign(userInput)
                || countSpaces(instr.info->format) != countSpaces(userInput)
                || sameParenthesis(userInput) != 0) {
                return EXIT_FAILURE;
            }

            countedArg = 0;
            for(int i=0; i<3; i++) {
                if(*(instr.info->srcs+i) == RD || *(instr.info->srcs+i) == RS || *(instr.info->srcs+i) == RT) {
                    // regs cannot be neg
                    if(*(instr.args+i)<0 || *(instr.args+i)>31){
                        return EXIT_FAILURE;
                    }
                    *(instr.regs + i) = *(instr.args+i);
                    countedArg++;
                } else if(*(instr.info->srcs+i) == EXTRA) {
                    instr.extra = *(instr.args+i);
                    countedArg++;
                }
            }

            if(countedArg != amtOfArg) {
                return EXIT_FAILURE;
            }

            if(encode(&instr, addr)) {
                if(endi) {
                    instr.value = bigEndian(instr.value);
                }

                for(int i = 4; i>0; i--) {
                    putchar(instr.value);
                    instr.value >>= 8;
                }
                addr += 4;
            }
            else {
                return EXIT_FAILURE;
            }
        }
    }

    // instr.regs[0] = 6;
    // instr.regs[1] = 31;
    // instr.regs[2] = 5;

    // instr.args[0] = 6;
    // instr.args[1] = 1235;
    // instr.args[2] = 5;
    // instr.extra = 12345;

    // Instr_info info[256] = {0};
    // info->opcode = OP_LBU;
    // info->type = ITYP;
    // info->srcs[0] = RT;
    // info->srcs[1] = EXTRA;
    // info->srcs[2] = RS;
    // info->format = "lbu $%d,%d($%d)";

    // instr.info = info;

    // printf("\nENCODE\n");
    // // encode test
    // if(!encode(&instr,addr)) {
    //     printf("ERROR at encode.");
    //     return EXIT_FAILURE;
    // }
    // printf("value: %x\n", instr.value);

    // printf("\nDECODE\n");
    // // Decode test
    // if(!decode(&instr, addr)) {
    //     printf("ERROR at decode.");
    //     return EXIT_FAILURE;
    // }
    // printf("value: %x\n", instr.value);
    // printf(instr.info->format, instr.args[0], instr.args[1], instr.args[2]);
    // printf("%s", "\n");

    // printf("op: %d\n", instr.info->opcode);
    // printf("%s\n", instr.info->format);
    // printf("Extra: %d\n", instr.extra);
    // printf("Arg 1: %d\n", instr.args[0]);
    // printf("Arg 2: %d\n", instr.args[1]);
    // printf("Arg 3: %d\n", instr.args[2]);
    // printf("Reg 1: %d\n", instr.regs[0]);
    // printf("Reg 2: %d\n", instr.regs[1]);
    // printf("Reg 3: %d\n", instr.regs[2]);

    return EXIT_SUCCESS;
}

/*
 * Just a reminder: All non-main functions should
 * be in another file not named main.c
 */
