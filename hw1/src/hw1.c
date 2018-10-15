#include <stdlib.h>

#include "debug.h"
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
 * @brief Checks to make sure 2 strings are equal.
 *
 * @param word1 is the address of the first word
 * @param word2 is the address of the second word
 *
 * @return 1 if strings are equal
 *         0 if strings are not equal
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
 * @brief Counts the length of a string
 *
 * @param str a string to be counted
 *
 * @return length of a char array
 */
int stringLength(char* str) {
    if(*str == '\0')
        return 0;
    return 1 + stringLength(++str);
}

/**
 * @brief Same as atoi (Converts string to integer)
 *
 * @param str is an array of chars which is consisted of decimal digits
 *
 * @return decimal value if string given is a decimal
 *          -1 if str is not a decimal value
 */
int decStrToDec(char* str) {
    int num = 0;
    int sign = 1;

    if(*str == '-') {
        sign = -1;
        str++;
    }

    while(*str != '\0' && *str>47 && *str<58) {
        num = num * 10 + (*str-48);
        str++;
    }

    if(*str=='\0') {
        return num * sign;
    }

    return -1;
}

/**
 * @brief Converts FACTOR from a string to a decimal value. Value is between 1 and 1024 inclusive
 *
 * @param str is a string value to be converted
 *
 * @return 1 if FACTOR value is 1-1024 inclusive
 *         0 o.w.
 */
int factor(char* str) {
    long int decimalValue = decStrToDec(str);
    if(decimalValue>0 && decimalValue<1025) {
        decimalValue -= 1;
        decimalValue <<= 48;
        global_options |= decimalValue;
        return 1;
    }
    global_options = 0;
    return 0;
}

/**
 * @brief Checks to see if string is a hex
 *
 * @param str is an array of chars
 *
 * @return 1 if is hex value
 *         0 o.w.
 */
int isHex(char* str) {
    if(*str == '\0')
        return 1;
    else if((*str>47 && *str < 58) || (*str>64 && *str<71) || (*str>96 && *str<103))
        return isHex(++str);
    else
        return 0;
}

/**
 * @brief Converts hex char array to decimal value (Pos num only)
 *
 * @param str is an array of chars which is consisted of hex digits
 *
 * @return decimal value if string given is a hex value
 *          -1 o.w.
 */
int hexStrToDec(char* str) {
    unsigned int num = 0;

    while(*str != '\0') {
        // btw 0-9
        if(*str>47 && *str < 58)
            num = num * 16 + (*str-48);
        // btw A-F
        else if(*str>64 && *str<71)
            num = num * 16 + (*str-65+10);
        // btw a-f
        else if(*str>96 && *str<103)
            num = num * 16 + (*str-97+10);
        else
            return -1;
        str++;
    }
    return num;
}

/**
 * @brief Checks to see if key is a hex value & length max is 8
 *
 * @param str is a string value to be checked
 *
 * @return 1 if str value is a hex value & length is at max 8
 *         0 o.w.
 */
int key(char* str) {
    unsigned int num = hexStrToDec(str);
    if(stringLength(str) > 8 || !isHex(str) || num < 0) {
        global_options = 0;
        return 0;
    }
    global_options |= num;
    return 1;
}

/**
 * @brief if -u or -d was passed go here to modify and check arguments
 *
 * @param argc is the amount of arguments passed
 * @param argv is an array of arguments
 *
 * @return 1 if valid
 *         0 o.w.
 */
int speed(int argc, char **argv){
    switch(argc){
        case 3:         // check if -p
            if(stringEquals("-p", *(argv+2))) {
                global_options |= 0x0800000000000000;
                return 1;
            }
            break;
        case 4:         // check if -f FACTOR
            if(stringEquals("-f", *(argv+2))) {
                return factor(*(argv+3));
            }
            break;
        case 5:         // check if -f FACTOR and -p in any order
            if(stringEquals("-p", *(argv+2)) && stringEquals("-f",*(argv+3))) {
                global_options |= 0x0800000000000000;
                return factor(*(argv+4));
            } else if(stringEquals("-p", *(argv+4)) && stringEquals("-f",*(argv+2))) {
                global_options |= 0x0800000000000000;
                return factor(*(argv+3));
            }
            break;
    }
    return 0;
}

/**
 * @brief if -c was passed go here to modify and check arguments
 *
 * @param argc is the amount of arguments passed
 * @param argv is an array of arguments
 *
 * @return 1 if valid
 *         0 o.w.
 */
int crypt(int argc, char **argv) {
    switch(argc){
        case 4:         // check if -c -k KEY
            if(stringEquals("-k",*(argv+2))) {
                return key(*(argv+3));
            }
            break;
        case 5:         // check if -c -k KEY -p in any order
            if(stringEquals("-p",*(argv+2)) && stringEquals("-k", *(argv+3))) {
                global_options |= 0x0800000000000000;
                return key(*(argv+4));
            } else if(stringEquals("-p",*(argv+4)) && stringEquals("-k", *(argv+2))) {
                global_options |= 0x0800000000000000;
                return key(*(argv+3));
            }
            break;
    }
    return 0;
}

/*
 * You may modify this file and/or move the functions contained here
 * to other source files (except for main.c) as you wish.
 *
 * IMPORTANT: You MAY NOT use any array brackets (i.e. [ and ]) and
 * you MAY NOT declare any arrays or allocate any storage with malloc().
 * The purpose of this restriction is to force you to use pointers.
 * Variables to hold the content of three frames of audio data and
 * two annotation fields have been pre-declared for you in const.h.
 * You must use those variables, rather than declaring your own.
 * IF YOU VIOLATE THIS RESTRICTION, YOU WILL GET A ZERO!
 *
 * IMPORTANT: You MAY NOT use floating point arithmetic or declare
 * any "float" or "double" variables.  IF YOU VIOLATE THIS RESTRICTION,
 * YOU WILL GET A ZERO!
 */

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 1 if validation succeeds and 0 if validation fails.
 * Upon successful return, the selected program options will be set in the
 * global variables "global_options", where they will be accessible
 * elsewhere in the program.
 *
 * @param argc
 *          The number of arguments passed to the program from the CLI.
 * @param argv
 *          The argument strings passed to the program from the CLI.
 * @return  1 if validation succeeds
 *          0 if validation fails.
 * Refer to the homework document for the effects of this function on
 * global variables.
 * @modifies global variable "global_options" to contain a bitmap representing
 * the selected options.
 */
int validargs(int argc, char **argv) {
    // No flags are provided
    if(argc < 2) {
        global_options = 0;
        return 0;
    } else if(stringEquals("-h", *(argv+1))) {
        global_options = 0x8000000000000000;
        return 1;
    } else if(stringEquals("-u", *(argv+1))) {
        global_options = 0x4000000000000000;
        if(argc==2) {
            return 1;
        } else {
            return speed(argc, argv);
        }
    } else if(stringEquals("-d", *(argv+1))) {
        global_options = 0x2000000000000000;
        if(argc==2) {
            return 1;
        } else {
            return speed(argc, argv);
        }
    } else if(stringEquals("-c", *(argv+1))) {
        global_options = 0x1000000000000000;
        return crypt(argc, argv);
    }
    global_options = 0;
    return 0;
}

/**
 * @brief Reads 4 bytes and returns it as an unsigned int
 *
 * @return  unsigned int value
 */
int readBytes(unsigned int* hp) {
    // result = getchar()<<24 | getchar()<<16 | getchar()<<8 | getchar();
    for(int i=0; i<4; i++) {
        *hp <<= 8;
        int temp = getchar();
        if(temp == EOF)
            return 0;
        *hp |= temp;
    }
    return 1;
}

/**
 * @brief Read the header of a Sun audio file and check it for validity.
 *
 * @details  This function reads 24 bytes of data from the standard input and
 * interprets it as the header of a Sun audio file. The data is decoded into
 * six unsigned int values, assuming big-endian byte order.   The decoded values
 * are stored into the AUDIO_HEADER structure pointed at by hp.
 *
 * The header is then checked for validity, which means:  no error occurred
 * while reading the header data, the magic number is valid, the data offset
 * is a multiple of 8, the value of encoding field is one of {2, 3, 4, 5},
 * and the value of the channels field is one of {1, 2}.
 *
 * @param hp  A pointer to the AUDIO_HEADER structure that is to receive
 * the data.
 * @return  1 if a valid header was read, otherwise 0.
 */
int read_header(AUDIO_HEADER *hp) {
    if(!readBytes(&hp->magic_number))
        return 0;
    if(!readBytes(&hp->data_offset))
        return 0;
    if(!readBytes(&hp->data_size))
        return 0;
    if(!readBytes(&hp->encoding))
        return 0;
    if(!readBytes(&hp->sample_rate))
        return 0;
    if(!readBytes(&hp->channels))
        return 0;

    //  Magic number = 2e736e64             data offset % 8
    if(hp->magic_number != 0x2e736e64 || hp->data_offset%8 != 0)
        return 0;
    // encoding = 2,3,4, or 5
    switch(hp->encoding){
        case(2):
        case(3):
        case(4):
        case(5):
            break;
        default:
            return 0;
    }
    //        channel = 1 or 2
    if(hp->channels != 1 && hp->channels != 2)
        return 0;

    return 1;
}

/**
 * @brief Writes 4 bytes
 *
 * @param hp the value to be printed
 */
int writeBytes(unsigned int val) {
    if(putchar(val>>24) == EOF)
        return 0;
    if(putchar(val>>16) == EOF)
        return 0;
    if(putchar(val>>8) == EOF)
        return 0;
    if(putchar(val) == EOF)
        return 0;
    return 1;
}

/**
 * @brief  Write the header of a Sun audio file to the standard output.
 * @details  This function takes the pointer to the AUDIO_HEADER structure passed
 * as an argument, encodes this header into 24 bytes of data according to the Sun
 * audio file format specifications, and writes this data to the standard output.
 *
 * @param  hp  A pointer to the AUDIO_HEADER structure that is to be output.
 * @return  1 if the function is successful at writing the data; otherwise 0.
 */
int write_header(AUDIO_HEADER *hp) {
    if(!writeBytes(hp->magic_number))
        return 0;
    if(!writeBytes(hp->data_offset))
        return 0;
    if(!writeBytes(hp->data_size))
        return 0;
    if(!writeBytes(hp->encoding))
        return 0;
    if(!writeBytes(hp->sample_rate))
        return 0;
    if(!writeBytes(hp->channels))
        return 0;
    return 1;
}

/**
 * @brief  Read annotation data for a Sun audio file from the standard input,
 * storing the contents in a specified buffer.
 *
 * @details  This function takes a pointer 'ap' to a buffer capable of holding at
 * least 'size' characters, and it reads 'size' characters from the standard input,
 * storing the characters read in the specified buffer.  It is checked that the
 * data read is terminated by at least one null ('\0') byte.
 *
 * @param  ap  A pointer to the buffer that is to receive the annotation data.
 * @param  size  The number of bytes of data to be read.
 * @return  1 if 'size' bytes of valid annotation data were successfully read;
 * otherwise 0.
 *
 * size=0 is ok
 *
 * pad into ap until size is mod by 8
 *
 * if last
 *      size % 8
 *      last element != 0
 *      error
 *
 */
int read_annotation(char *ap, unsigned int size) {
    // Can only have 1024 bits with \0 being the last char
    if(size>1023)
        return 0;
    if(size==0)
        return 1;

    for(unsigned int i=0; i<size; i++) {
        int temp = getchar();

        // Last char must be null terminator
        if(i==size-1 && temp!='\0')
            return 0;
        else if(temp == EOF)
            return 0;
        *(ap+i) = temp;
    }
    return 1;
}

/**
 * @brief  Write annotation data for a Sun audio file to the standard output.
 * @details  This function takes a pointer 'ap' to a buffer containing 'size'
 * characters, and it writes 'size' characters from that buffer to the standard
 * output.
 *
 * @param  ap  A pointer to the buffer containing the annotation data to be
 * written.
 * @param  size  The number of bytes of data to be written.
 * @return  1 if 'size' bytes of data were successfully written; otherwise 0.
 */
int write_annotation(char *ap, unsigned int size) {
    for(unsigned int i=0; i<size; i++){
        putchar(*(ap+i));
    }
    return 1;
}

/**
 * @brief Read, from the standard input, a single frame of audio data having
 * a specified number of channels and bytes per sample.
 *
 * @details  This function takes a pointer 'fp' to a buffer having sufficient
 * space to hold 'channels' values of type 'int', it reads
 * 'channels * bytes_per_sample' data bytes from the standard input,
 * interpreting each successive set of 'bytes_per_sample' data bytes as
 * the big-endian representation of a signed integer sample value, and it
 * stores the decoded sample values into the specified buffer.
 *
 * @param  fp  A pointer to the buffer that is to receive the decoded sample
 * values.
 * @param  channels  The number of channels.
 * @param  bytes_per_sample  The number of bytes per sample.
 * @return  1 if a complete frame was read without error; otherwise 0.
 */
// int read_frame(int *fp, int channels, int bytes_per_sample) {
//     int size = channels*bytes_per_sample;
//     int temp;
//     for(int i=0; i<size; i++) {
//         temp = getchar();
//         *(fp+i) = temp;
//         // *((int*)fp) <<= 8;
//         // *fp = temp;
//     }
//     return 1;
// }

int read_frame(int *fp, int channels, int bytes_per_sample) {
    for(int i=0; i<channels*bytes_per_sample; i++) {
        int temp = getchar();
        if(temp == EOF)
            return 0;
        *fp <<= 8;
        *fp |= temp;
    }
    return 1;
}


/**
 * @brief  Write, to the standard output, a single frame of audio data having
 * a specified number of channels and bytes per sample.
 *
 * @details  This function takes a pointer 'fp' to a buffer that contains
 * 'channels' values of type 'int', and it writes these data values to the
 * standard output using big-endian byte order, resulting in a total of
 * 'channels * bytes_per_sample' data bytes written.
 *
 * @param  fp  A pointer to the buffer that contains the sample values to
 * be written.
 * @param  channels  The number of channels.
 * @param  bytes_per_sample  The number of bytes per sample.
 * @return  1 if the complete frame was written without error; otherwise 0.
 */
int write_frame(int *fp, int channels, int bytes_per_sample) {
    int size = channels*bytes_per_sample;
    for(int i=size-1; i>=0; i--) {
        if(putchar(*((int*)fp)>>i*8) == EOF)
            return 0;
    }
    return 1;
}

/**
 * @details  The annotation for the output file is constructed by prepending to
 * the input annotation a representation of the command-line arguments given to
 * "audible". This representation is produced by concatenatenating the strings
 * from the "argv" array, separating successive arguments by a single space
 * (' ') char, and terminating the result with a single newline ('\n') char. If
 * the input audio file had no annotation field, then the output file will have
 * an annotation that consists only of the audible command line, followed by at
 * least one null ('\0') byte. If the input file did have a nonempty annotation
 * field, then the annotation field of the output file will consist of the
 * ('\n'-terminated) audible command line, immediately followed by the original
 * annotation from the input file, without any intervening null byte. In all
 * cases, the output annotation shall conform to the Sun audio file format
 * specifications.
 */
int prepending(char **argv){
    int size_output = 0;
    // add in annotation
    while(*argv!='\0') {
        char* str = *argv;
        while(*str!='\0') {
            *(output_annotation+size_output) = *str++;
            size_output++;
        }
        if(*++argv!='\0') {
            *(output_annotation+size_output) = ' ';
            size_output++;
        }
    }
    *(output_annotation+size_output) = '\n';
    size_output++;

    int size_input = 0;
    // add in original annotation
    while(*(input_annotation+size_input)!='\0') {
        *(output_annotation+size_output+size_input) = *(input_annotation+size_input);
        size_input++;
    }
    *(output_annotation+size_output+size_input) = '\0';

    return size_output;
}

void copyFrame(int* one, int* two) {
    for(int i = 0; i < CHANNELS_MAX * sizeof(int); i++)
        *one++ = *two++;
}

// int twosComp(int* frame, int size) {
//     unsigned int val = *frame;
//     // get msb
//     int temp = val << 1;
//     temp = temp >> 1;
//     if(val==temp) // pos
//         return val;

//     val<<=1;    // remove the sign bit
//     unsigned int sum = 0;
//     for(int i=0; i<size-1; i++){
//         int temp = val>>i;
//         if(temp&0x1)     // if there is a 1
//             sum = sum * 2;
//         else
//             sum = sum * 2 + 1;
//     }
//     sum += 1;
//     return sum*-1;
// }

/**
 * @brief  Recodes a Sun audio (.au) format audio stream, reading the stream
 * from standard input and writing the recoded stream to standard output.
 *
 * @details  This function reads a sequence of bytes from the standard
 * input and interprets it as digital audio according to the Sun audio
 * (.au) format.  A selected transformation (determined by the global variable
 * "global_options") is applied to the audio stream and the transformed stream
 * is written to the standard output, again according to Sun audio format.
 *
 * Besides the transformation applied to the audio sample data, unless the -p
 * option was specified, the annotation data is transformed by prepending a
 * representation of the command-line arguments given to "audible".
 * If the transformation annotation would be longer than ANNOTATION_MAX,
 * it shall be considered an error and this function shall return 0 without
 * producing any output. If the -p option was specified, then no transformation
 * is performed on the annotation, and the output annotation is identical to
 * the input annotation.
 *
 * @param  argv  Command-line arguments, for constructing modified annotation.
 * @return 1 if the recoding completed successfully, 0 otherwise.
 */
int recode(char **argv) {
    int noerror = 1;
    // Checks if -p was given
    int p = global_options >> 59 & 0x00000001;

    AUDIO_HEADER hp;
    noerror &= read_header(&hp);
    unsigned int annotation_size = hp.data_offset-sizeof(hp);
    if(annotation_size<0 || annotation_size%8!=0 || annotation_size>ANNOTATION_MAX)
        return 0;

    noerror &= read_annotation(input_annotation, annotation_size);

    if(!noerror)
        return 0;

    unsigned int factor = (global_options>>48)&0x03FF;
    factor++;

    // Speed up -u
    if(global_options&0x4000000000000000) {
        int frame = hp.data_size/(hp.channels*(hp.encoding-1));
        frame = (frame+1);
        hp.data_size = frame*factor;
    } else if(global_options&0x2000000000000000) {  // slow down
        hp.data_size /= factor;
        hp.data_size = hp.data_size - 1;
        hp.data_size = hp.data_size * (hp.channels*(hp.encoding-1));
    }

    if(!p) {
        int tempDef = prepending(argv);
        annotation_size += tempDef;

        // pad with 0
        while(annotation_size%8!=0) {
            annotation_size++;
            (*input_annotation)++;
            *(input_annotation+annotation_size) = '\0';
        }
        hp.data_offset = annotation_size + sizeof(hp);


        if(tempDef > ANNOTATION_MAX || hp.data_offset<0)
            return 0;

        noerror &= write_header(&hp);
        noerror &= write_annotation(output_annotation, annotation_size);
    } else {
        noerror &= write_header(&hp);
        noerror &= write_annotation(input_annotation, annotation_size);
    }

    if(!noerror)
        return 0;

    // Speed up -u
    if(global_options&0x4000000000000000) {
        for(int i=0; i<hp.data_size/factor; i++) {
            read_frame((int*)input_frame, hp.channels, hp.encoding-1);
            if(i%factor==0)
                noerror &= write_frame((int*)input_frame, hp.channels, hp.encoding-1);
        }
    }

    // Slow down -d
    if(global_options&0x2000000000000000) {
        read_frame((int*)input_frame, hp.channels, factor);
        noerror &= write_frame((int*)input_frame, hp.channels, factor);

        if(hp.channels == 1) {
            for(int i=0; i<hp.data_size/(factor*(hp.encoding+1)); i++) {
                copyFrame((int*)previous_frame, (int*)input_frame);
                noerror &= read_frame((int*)input_frame, hp.channels, hp.encoding-1);

                int s = *previous_frame;
                int t = *input_frame;

                // copyFrame(*output_frame,s+(t-s)*i/factor);
                *((int*)output_frame) = (s+(t-s)*i)/factor;
                // print new
                noerror &= write_frame((int*)output_frame, hp.channels, hp.encoding-1);
                // print next
                noerror &= write_frame((int*)input_frame, hp.channels, hp.encoding-1);
            }
        } else {        // channel = 2
            for(int i=0; i<hp.data_size/(factor*(hp.encoding+1)); i++) {
                copyFrame((int*)previous_frame, (int*)input_frame);
                noerror &= read_frame((int*)input_frame, hp.channels, hp.encoding-1);

                int s1 = *(int*)previous_frame>>24;
                int s2 = *(int*)previous_frame>>8;
                int t1 = *(int*)input_frame>>24;
                int t2 = *(int*)input_frame>>8;
                s1 &= 0xFF;
                s2 &= 0xFF;
                t1 &= 0xFF;
                t2 &= 0xFF;

                debug("%x",((s1+(t2-s1)*(i+1)/factor)));
                debug("%x\n",(s2+(t1-s2)*(i+1)/factor));

                *((int*)output_frame) = ((s1+(t1-s1)*(i+1)/factor)<<8)
                                       |((s2+(t2-s2)*(i+1)/factor));

                *((int*)output_frame) <<= 16;




// for(int i=0; i<hp.data_size; i+=factor) {
//             copyFrame((int*)previous_frame, (int*)input_frame);
//             read_frame((int*)input_frame, hp.channels, hp.encoding-1);

//             *input_frame&=factor;
//             int s = twosComp((int*)previous_frame);
//             int t = twosComp((int*)input_frame);

//             output_frame=s+(t-s)*i/factor;
//             write_frame((int*)output_frame, hp.channels, hp.encoding-1);
//             write_frame((int*)input_frame, hp.channels, hp.encoding-1);
//         }




                s1 = *(int*)previous_frame>>16;
                s2 = *(int*)previous_frame&0xFF;
                t1 = *(int*)input_frame>>16;
                t2 = *(int*)input_frame&0xFF;
                s1 &= 0xFF;
                s2 &= 0xFF;
                t1 &= 0xFF;
                t2 &= 0xFF;

                *((int*)output_frame) |= ((s1+(t1-s1)*(i+1)/factor)<<8)
                                        |((s2+(t2-s2)*(i+1)/factor));

                // print new
                noerror &= write_frame((int*)output_frame, hp.channels, hp.encoding-1);
                // print next
                noerror &= write_frame((int*)input_frame, hp.channels, hp.encoding-1);
            }
        }
    }

    // Crypt
    if(global_options&0x1000000000000000) {
        unsigned int key = global_options & 0xFFFFFFFF;
        mysrand(key);

        for(int i=0; i<hp.data_size/(hp.channels*(hp.encoding-1)); i++) {
            read_frame((int*)input_frame, hp.channels, hp.encoding-1);

            if(hp.encoding == 2){
                if(hp.channels==2)
                    *((int*)output_frame) = (((myrand32() & 0xFF) << 24)|(myrand32() & 0xFF));
                else
                    *((int*)output_frame) = (myrand32() & 0xFF);

            } else if (hp.encoding == 3){
                if(hp.channels==2)
                    *((int*)output_frame) = (((myrand32() & 0xFFFF) << 16)|(myrand32() & 0xFFFF));
                else
                    *((int*)output_frame) = (myrand32() & 0xFFFF);

            } else if (hp.encoding == 4){
                if(hp.channels==2)
                    *((int*)output_frame) = (((myrand32() & 0xFFFFFF) << 8)|(myrand32() & 0xFFFFFF));
                else
                    *((int*)output_frame) = (myrand32() & 0xFFFFFF);

            } else if (hp.encoding == 5){
                if(hp.channels==2)
                    *((int*)output_frame) = ((myrand32() & 0xFFFFFFFF)|(myrand32() & 0xFFFFFFFF));
                else
                    *((int*)output_frame) = (myrand32() & 0xFFFFFFFF);
            }

            *((int*)output_frame) ^= *((int*)input_frame);
            noerror &= write_frame((int*)output_frame, hp.channels, hp.encoding-1);
        }
    }

    if(!noerror)
        return 0;
    return 1;
}
// bin/audible -c -k DeadBeef -p < rsrc/sample_c_DeadBeef.au > out.au