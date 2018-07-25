/**
 * You may modify this file and/or move the functions contained here
 * to other source files (except for main.c) as you wish.
 */
#include "hw1.h"

#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

/**
 * @brief Changes the num to bigEndian. Used in main.c
 *
 * @param an unsigned int to be converted to bigEndian
 * @return the number in big endian form
 */
int bigEndian(unsigned int num) {
    unsigned int temp1 = 0;
    unsigned int temp2 = 0;

    // Swap the first and last
    temp1 = num << 24;
    temp2 = num >> 24;

    // Remove the first and last 8 bits
    num &= 0x00FFFF00;
    num |= temp1;
    num |= temp2;

    // Swap the middle 2
    temp1 = num >> 8;
    temp2 = num << 8;

    temp1 &= 0x0000FF00;
    temp2 &= 0x00FF0000;

    // Remove the middle values
    num &= 0xFF0000FF;
    num |= temp1;
    num |= temp2;

    return num;
}

/**
 * @brief Checks to make sure 2 strings are equal. Used in main.c
 *
 * @param word1 is the address of the first word
 * @param word2 is the address of the second word
 *
 * @return  1 if strings are equal
 *           0 if strings are not equal
 */
int stringEqual(char* word1, char* word2) {
    // End of string. String are equal
    if(*word1 == '\0' && *word2 == '\0')
        return 1;
    else if(*word1 == '\n' && *word2 == '\0')
        return 1;
    // Make sure the current char of both strings are equal
    else if(*word1 == *word2)
        return stringEqual(++word1, ++word2);
    // Strings are not equal.
    else
        return 0;
}

int countCommas(char* word) {
    if(*word == '\0' || *word == '\n') {
        return 0;
    }
    // Make sure the current char of both strings are equal
    else if(*word == ',') {
        return 1 + countCommas(++word);
    }
    else {
        return 0 + countCommas(++word);
    }
}

int countDollaSign(char* word) {
    if(*word == '\0' || *word == '\n') {
        return 0;
    }
    // Make sure the current char of both strings are equal
    else if(*word == '$') {
        return 1 + countDollaSign(++word);
    }
    else {
        return 0 + countDollaSign(++word);
    }
}

int countSpaces(char* word) {
    if(*word == '\0' || *word == '\n') {
        return 0;
    }
    // Make sure the current char of both strings are equal
    else if(*word == ' ') {
        return 1 + countSpaces(++word);
    }
    else {
        return 0 + countSpaces(++word);
    }
}

int sameParenthesis(char* word) {
    if(*word == '\0' || *word == '\n') {
        return 0;
    }
    // Make sure the current char of both strings are equal
    else if(*word == '(') {
        return 1 + sameParenthesis(++word);
    }
    else if(*word == ')') {
        return -1 + sameParenthesis(++word);
    } else {
        return sameParenthesis(++word);
    }
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
 * @brief Checks to see if a string of hex is divisible by 4096
 *
 * @param num is the string to check for last 3 letter. It should be in hex. (postcondition)
 * @param length is the length of the string
 *
 * Postcondition: The values passed in are all correct.
 *
 * @return 0 if the num is not divisble by 4096. Last 3 digits are not 000
          1 o.w.
 */
int lastThreeZero(char* num, int length) {
    // We do not need this if statement. But I put it there just in case.
    if(length < 1)
        return 0;
    if(length > 3)
        // Get the last 3 digits
        num += length - 3;

    while(*num != '\0') {
        if(*num != '0')
            return 0;
        num++;
    }
    return 1;
}

/**
 * @details Invalid base address (if one is specified).
 * @details A base address is invalid if it contains characters other than the digits
 * ('0'-'9'), upper-case letters ('A'-'F'), and lower-case letters ('a'-'f'). If it is
 * more than 8 digits length, or if it is not a multiple of 4096. (i.e. the twelve
 * least-significant bits of its value are not all zero).
 *
 * @param num is the address of the base address
 *
 * @return 0 if num is not a valid base address
 *         1 o.w.
 */
int validBase(char* num) {
    // Loop for 9 turns. #### #### '\0'
    for(int i = 0; i < 9; i++) {
        if(*num == '\0') {
            num -= i;
            return lastThreeZero(num, i);
        }
        //       0=48 9=57             a=97 f=102               A=65 F=70
        if(!((*num>47 && *num<58) || (*num>96 && *num<103) || (*num>64 && *num<71))) {
            return 0;
        }
        num++;
    }
    return 0;
}

/**
 * @brief Convert the given string to an integer.
 *
 * @param num The string to convert to integer
 *
 * @return the integer (int decimal) of the string num
 *         -1 if value is not a hex.
 */
unsigned int strToI(char* num) {
    unsigned int result = 0;
    while(1) {
        // 0-9
        if(*num>47 && *num<58){
            result += (*num - 48);
        }
        // a-f
        else if(*num>96 && *num<103) {
            result += (*num - 87);
        }
        // A-F
        else if(*num>64 && *num<71) {
            result += (*num - 55);
        } else {
            return 0;
        }

        num++;
        if(*num == '\0')
            return result;
        result <<= 4;
    }
}

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 1 if validation succeeds and 0 if validation fails.
 * Upon successful return, the selected program options will be set in the
 * global variable "global_options", where they will be accessible
 * elsewhere in the program.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 *
 * @return 1 if validation succeeds and 0 if validation fails.
 * Refer to the homework document for the effects of this function on
 * global variables.
 *
 * @modifies global variable "global_options" to contain a bitmap representing
 * the selected options.
 */
int validargs(int argc, char **argv)
{
    // initialize
    global_options = 0;

    // No flags are provided
    if(argc < 2) {
        return 0;
    }
    // -h flag is provided
    if(stringEquals("-h", *(argv+1))) {
        global_options = 1;
        return 1;
    }

    // -a flag is provided, perform text-to-binary conversion
    if(stringEquals("-a", *(argv+1))) {
        // Second least significant bit is 0
        global_options &= 00;
        if(argc < 3) { // Only -a flag
            return 1;
        } else if(argc == 4 && stringEquals("-b", *(argv + 2))) {  // -a -b BASEADDR
            // base-address checker
            if(!validBase(*(argv + 3))) {
                // invalid base address
                return 0;
            } else {
                // base address specified.
                global_options |= strToI(*(argv + 3));
                return 1;
            }
        } else if(argc == 4 && stringEquals("-e", *(argv + 2))) { // -a -e ENDDIANNESS
            // Checking ENDDIANNESS
            if(stringEquals("b", *(argv + 3))) { // ENDDIANNESS = big-endian
                // third significant bit is 1. 100(binary) = 4(hex)
                global_options |= 0x4;
                return 1;
            } else if(stringEquals("l", *(argv + 3))) { // ENDDIANNESS = little-endian
                return 1;
            } else {
                // ENDDIANNESS given is not correct.
                return 0;
            }
        } else if(argc == 6 && stringEquals("-e", *(argv + 2)) && stringEquals("-b", *(argv + 4))) { // -a -e ENDDEIANNES -b BASEADDR

            // Checking ENDDIANNESS
            if(stringEquals("b", *(argv + 3))) { // ENDDIANNESS = big-endian
                // third significant bit is 1. 100(binary) = 4(hex)
                global_options |= 0x4;
            } else if(!stringEquals("l", *(argv + 3))) { // ENDIANNESS = little-endian
                // ENDDIANNESS given is not correct.
                return 0;
            }

            if(!validBase(*(argv + 5))){
                // If -e endianness == b. We have a value set in global options. So reset it
                global_options = 0;
                return 0;
            } else {
                // base address specified.
                global_options |= strToI(*(argv + 5));
                return 1;
            }

        } else if(argc == 6 && stringEquals("-b", *(argv + 2)) && stringEquals("-e", *(argv + 4))) { // -a -b BASEADDR -e ENDDEIANNES
            if(!validBase(*(argv + 3))) {
                // invalid base address
                return 0;
            } else {
                // base address specified.
                global_options |= strToI(*(argv + 3));
            }

            // Checking ENDDIANNESS
            if(stringEquals("b", *(argv + 5))) {                 // ENDDIANNESS = big-endian
                // third significant bit is 1. 100(binary) = 4(hex)
                global_options |= 0x4;
                return 1;
            } else if(stringEquals("l", *(argv + 5))) {          // ENDDIANNESS = little-endian
                return 1;
            } else {
                global_options = 0;
                // ENDDIANNESS given is not correct.
                return 0;
            }
        } else {
            global_options = 0;
            // arguments given are incorrect, invalid number of arguments
            return 0;
        }
    }

    // -d flag is provided, perform binary-to-text conversion
    else if(stringEquals("-d", *(argv+1))) {
        // Second least significant bit is 1
        global_options = 2;
        if(argc < 3) { // Only -d flag
            return 1;
        } else if(argc == 4 && stringEquals("-b", *(argv + 2))) {  // -d -b BASEADDR
            // base-address checker
            if(!validBase(*(argv + 3))) {
                global_options = 0;
                // invalid base address
                return 0;
            } else{
                // base address specified.
                global_options |= strToI(*(argv + 3));
                return 1;
            }
        } else if(argc == 4 && stringEquals("-e", *(argv + 2))) { // -d -e ENDDIANNESS

            // Checking ENDDIANNESS
            if(stringEquals("b", *(argv + 3))) { // ENDDIANNESS = big-endian
                // third significant bit is 1. 100(binary) = 4(hex)
                global_options |= 0x4;
                return 1;
            } else if(stringEquals("l", *(argv + 3))) { // ENDDIANNESS = little-endian
                return 1;
            } else {
                global_options = 0;
                // ENDDIANNESS given is not correct.
                return 0;
            }

        } else if(argc == 6 && stringEquals("-e", *(argv + 2)) && stringEquals("-b", *(argv + 4))) { // -d -e ENDDEIANNES -b BASEADDR

            // Checking ENDDIANNESS
            if(stringEquals("b", *(argv + 3))) { // ENDDIANNESS = big-endian
                // third significant bit is 1. 100(binary) = 4(hex)
                global_options |= 0x4;
            } else if(stringEquals("l", *(argv + 3))) { // ENDDIANNESS = little-endian

            } else {
                global_options = 0;
                // ENDDIANNESS given is not correct.
                return 0;
            }

            if(!validBase(*(argv + 5))){
                global_options = 0;
                return 0;
            } else {
                // base address specified.
                global_options |= strToI(*(argv + 5));
                return 1;
            }

        } else if(argc == 6 && stringEquals("-b", *(argv + 2)) && stringEquals("-e", *(argv + 4))) { // -d -b BASEADDR -e ENDDEIANNES

            if(!validBase(*(argv + 3))) {
                global_options = 0;
                // invalid base address
                return 0;
            } else {
               // base address specified.
                global_options |= strToI(*(argv + 3));
            }

            // Checking ENDDIANNESS
            if(stringEquals("b", *(argv + 5))) {                 // ENDDIANNESS = big-endian
                // third significant bit is 1. 100(binary) = 4(hex)
                global_options |= 0x4;
                return 1;
            } else if(stringEquals("l", *(argv + 5))) {          // ENDDIANNESS = little-endian
                return 1;
            } else {
                global_options = 0;
                // ENDDIANNESS given is not correct.
                return 0;
            }
        } else {
            global_options = 0;
            // arguments given are incorrect, invalid number of arguments
            return 0;
        }
    } else {
        global_options = 0;
        // arguments given are incorrect
        return 0;
    }
}

/**
 * @brief Makes sure that optable has the SPECIAL opcode.
 *
 * @return 1 if special table has opcode
 *         0 o.w.
 */
int hasSpecial() {
    for(int i=0; i < NUM_ELEM; i++) {
        if(*(opcodeTable+i) == SPECIAL){
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Checks if an opcode is special.
 *
 * @param ip The Instruction structure containing information about the
 * instruction, except for the "value" field.
 *
 * @return 1 in specialTable if special
 *         0 o.w.
 *
 * @modifies the "value" field of the Instruction structure to contain the special
 * value if needed. If case is not special or dne, no modifications are made
 */
int isSpecial(Instruction *ip) {
    for(int i=0; i < NUM_ELEM; i++) {
        if(*(specialTable+i) == ip->info->opcode) {
            if(!hasSpecial())
                return 0;
            // index in special table 5:0
            ip->value |= i;
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Checks if an opcode is not special.
 *
 * @param ip The Instruction structure containing information about the
 * instruction, except for the "value" field.
 *
 * @return 1 in specialTable if not special
 *         0 o.w.
 *
 * @modifies the "value" field of the Instruction structure to contain the needed
 * value for non-special cases. If case is special or dne, no modifications are made
 */
int isNotSpecial(Instruction *ip) {
    // Not special case
    for(int i=0; i < NUM_ELEM; i++) {
        if(*(opcodeTable+i) == ip->info->opcode) {
            // Put the value of opcode into the value with the correct position
            ip->value = i << 26;
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Checks if the instruction is a branch.
 *
 * @param ip The Instruction structure containing information about the
 * instruction, except for the "value" field.
 *
 * @return 1 if the opcode is a branch
 *         0 o.w.
 */
int isBranch(Instruction *ip){
    switch(ip->info->opcode) {
        case OP_BEQ:
        case OP_BGEZ:
        case OP_BGEZAL:
        case OP_BGTZ:
        case OP_BLEZ:
        case OP_BLTZ:
        case OP_BLTZAL:
        case OP_BNE:
            return 1;
        default:
            return 0;
    }
}

/**
 * @brief Find the location of BCOND.
 * @details Place the location of BCOND into ip-value if found
 *
 * @return 1 if BCOND is found.
 *         0 o.w.
 */
int findBcond(Instruction *ip) {
    for(int i=0; i < NUM_ELEM; i++) {
        if(*(opcodeTable+i) == BCOND) {
            i <<= 26;
            ip->value|=i;
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Computes the binary code for a MIPS machine instruction.
 * @details This function takes a pointer to an Instruction structure
 * that contains information defining a MIPS machine instruction and
 * computes the binary code for that instruction. The code is returned
 * in the "value" field of the Instruction structure.
 *
 * @param ip The Instruction structure containing information about the
 * instruction, except for the "value" field.
 * @param addr Address at which the instruction is to appear in memory.
 * The address is used to compute the PC-relative offsets used in branch
 * instructions.
 *
 * @return 1 if the instruction was successfully encoded, 0 otherwise.
 *
 * @modifies the "value" field of the Instruction structure to contain the
 * binary code for the instruction.
 */
int encode(Instruction *ip, unsigned int addr) {
    if(ip->info->type == NTYP)
        return 0;
    else
        // initialize the value
        ip->value = 0;

    if(ip->info->opcode == OP_BGEZ && findBcond(ip)) {        // conditional branch instruction = op_code = BCOND
        // value at 20:16 is 00001
        ip->value|=0x10000;
    } else if(ip->info->opcode == OP_BLTZAL && findBcond(ip)) {
        // value at 20:16 is 10000
        ip->value|=0x100000;
    } else if(ip->info->opcode == OP_BGEZAL && findBcond(ip)) {
        // value at 20:16 is 10001
        ip->value|=0x110000;
    } else if(ip->info->opcode == OP_BLTZ && findBcond(ip)) {

    } else if(!isSpecial(ip) && !isNotSpecial(ip)){
        // An ivalid opcode. opcode is not special nor special
        // value at 20:16 is 00000 if opcode == OP_BLTZ
        return 0;
    }

    for(int i=0; i<3; i++) {
        // Copy over the arguments
        // *(ip->args+i) = *(ip->info->srcs)+i;

        if(*(ip->info->srcs+i) == EXTRA) {
            // if(ip->extra != *(ip->args+i))
            //     return 0;

            int extra = *(ip->args+i);
            // If the opcode is OP_BREAK, take 20-bits and put into 25:6
            if(ip->info->opcode == OP_BREAK) {
                // Remove any extra values just in case
                extra <<= 12;
                // Put into position
                extra >>= 6;
            }
            // instruction of type R take 5 bits put into 10:6
            else if(ip->info->type == RTYP) {
                // Take out the rest of the bits if there is any
                extra &= 0x1F;
                // Put into position
                extra <<= 6;
            }
            // insturction of type I take 16 bits put into 15:0
            else if(ip->info->type == ITYP){
                // instruction is a conditional branch Instructions  (sign-extend <<2) + (PC+4)
                if(isBranch(ip)) {
                    // Remove the address from the value
                    extra -= (addr + 4);
                    extra >>= 2;
                }
                if(extra != (ip->extra))
                    return 0;
                extra &=  0xFFFF;
                // Remove any extra values just in case
            }
            // instruction of type J 26bit<<2 + first4PC
            else if(ip->info->type == JTYP) {
                int myPC = ip->extra & 0xF0000000;
                int addrPC = addr & 0xF0000000;
                // Must have the same 4 msb
                if(myPC != addrPC)
                    return 0;

                // 4msb from pc
                int pc = (addr-4) & 0xF0000000;
                extra -= pc;
                extra >>= 2;
                extra &= 0x3FFFFFF;
            }
            ip->value |= extra;
        }
        // 25:21
        else if(*(ip->info->srcs+i) == RS) {
            int rs = *(ip->args+i);

            if(rs>31 || rs<0)
                return 0;

            rs <<= 21;
            // Make sure to only add in these values.
            rs &= 0x3E00000;
            ip->value |= rs;
        }
        // 20:16
        else if(*(ip->info->srcs+i) == RT) {
            int rt = *(ip->args+i);

            if(rt>31 || rt<0)
                return 0;

            rt <<= 16;
            rt &= 0x1F0000;
            ip->value |= rt;
        }
        // 15:11
        else if(*(ip->info->srcs+i) == RD) {
            int rd = *(ip->args+i);

            if(rd>31 || rd<0)
                return 0;

            rd <<= 11;
            rd &= 0xF800;
            ip->value |= rd;
        } else if(*(ip->info->srcs+i) == NSRC) {
            if(*(ip->args+i) != 0)
                return 0;
        } else {
            return 0;
        }
    }
    return 1;
}

/**
 * @brief Decodes the binary code for a MIPS machine instruction.
 * @details This function takes a pointer to an Instruction structure
 * whose "value" field has been initialized to the binary code for
 * MIPS machine instruction and it decodes the instruction to obtain
 * details about the type of instruction and its arguments.
 * The decoded information is returned by setting the other fields
 * of the Instruction structure.
 *
 * @param ip The Instruction structure containing the binary code for
 * a MIPS instruction in its "value" field.
 * @param addr Address at which the instruction appears in memory.
 * The address is used to compute absolute branch addresses from the
 * the PC-relative offsets that occur in the instruction.
 *
 * @return 1 if the instruction was successfully decoded, 0 otherwise.
 *
 * @modifies the fields other than the "value" field to contain the
 * decoded information about the instruction.
 */
int decode(Instruction *ip, unsigned int addr) {
    int op = (unsigned int) ip->value >> 26;
    // Set it to the first element of the table. For space
    Opcode opcode =*specialTable;

    // SPECIAL
    if(*(opcodeTable+op) == SPECIAL) {
        // Get the last 6 values
        op = ip->value & 0x3F;
        if(specialTable[op] == ILLEGL || specialTable+op == NULL)
            return 0;
        // 31:26 special 5:0 opcode
        opcode = *(specialTable+op);
        // ip->info->opcode = *(specialTable+op);
    }
    // BCOND
    else if(*(opcodeTable+op) == BCOND) {
        // get the opcode for bcond
        op = (ip->value >> 16) & 0x1F;
        if(op == 0) {
            opcode = OP_BLTZ;
        } else if(op == 1) {
            opcode = OP_BGEZ;
        } else if(op == 16) {
            opcode = OP_BLTZAL;
        } else if(op == 17) {
            opcode = OP_BGEZAL;
        } else {
            return 0;
        }
    }
    // Within bound and is not illegl. Then it is an opcode
    else if(opcodeTable+op != NULL && *(opcodeTable+op) != ILLEGL) {
        opcode = *(opcodeTable+op);
    } else {
        return 0;
    }

    // Copy instr_info using obtained opcode
    for(int i=0; i < NUM_ELEM; i++) {
        if(opcode == (instrTable+i)->opcode) {
            ip->info = instrTable+i;
            break;
        }
    }

    // Value in each reg is rs,rt,rd respectively
    ip->regs[0] = ((ip->value & 0x03e00000) >> 21);
    ip->regs[1] = ((ip->value & 0x001f0000) >> 16);
    ip->regs[2] = ((ip->value & 0x0000f800) >> 11);

    for(int i=0; i<3; i++) {
        if(*(ip->info->srcs+i) == EXTRA) {
            // All extras start with the value.
            ip->extra = ip->value;
            // extra and op_break
            if(opcode == OP_BREAK) {
                ip->extra >>= 6 ;
                ip->extra  &= 0x000FFFFF;
            } else if(ip->info->type == RTYP) {
                ip->extra >>= 6;
                ip->extra  &= 0x0000001F;
            } else if(ip->info->type == ITYP) {
                int sign = ip->extra >> 15;
                sign &= 0x00000001;
                // Sign extend
                if(sign) {
                    // Make sure the last 16 binary digits are all 1
                    ip->extra |= 0xFFFF0000;
                } else {
                    // Make sure the last 16 binary digits are all 0
                    ip->extra &= 0x0000FFFF;
                }
                // addtional steps
                if(isBranch(ip)) {
                    // absoluate address which is the branch target
                    ip->extra <<= 2;
                    ip->extra += addr + 4;
                }
            } else if(ip->info->type == JTYP) {
                ip->extra &= 0x03FFFFFF;
                ip->extra <<= 2;
                // 4 msb of address
                ip->extra += (addr + 4) & 0xF0000000;
            } else {
                // Extra but no where to put it
                return 0;
            }
            *(ip->args+i) =  ip->extra;
        }else if(*(ip->info->srcs+i) == RS) {
            int rs = ip->value;
            rs >>= 21;
            // Keep the last 5 digits
            *(ip->args+i) =  rs & 0x1F;
        } else if(*(ip->info->srcs+i) == RT) {
            int rt = ip->value;
            rt >>= 16;
            // Keep the last 5 digits
            *(ip->args+i) =  rt & 0x1F;
        } else if(*(ip->info->srcs+i) == RD) {
            int rd = ip->value;
            rd >>= 11;
            // Keep the last 5 digits
            *(ip->args+i) =  rd & 0x1F;
        } else if(*(ip->info->srcs+i) == NSRC){
            *(ip->args+i) = 0;
        } else {
            return 0;
        }
    }
    return 1;
}