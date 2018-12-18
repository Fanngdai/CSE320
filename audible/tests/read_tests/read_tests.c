#include <criterion/criterion.h>
#include <criterion/logging.h>

#include "__read_helpers.h"
#include "hw1.h"
#include "const.h"

////////////////////////////////////////////
// HEADER TESTS
////////////////////////////////////////////

Test(read_tests_suite, invalid_header_offset_not_multiple_8_test) {
    open_input_file("./tests/rsrc/invalid_header_offset_not_multiple_8");

    AUDIO_HEADER hp;
    int ret = read_header(&hp);
    int exp_ret = 0;

    cr_expect_eq(ret, exp_ret, "Invalid return for read header.  Got: %d | Expected: %d", ret, exp_ret);
}

////////////////////////////////////////////
// ANNOTATION TESTS
////////////////////////////////////////////
// read a simple annotation
Test(read_tests_suite, read_annotation_test_basic){
    open_input_file("./tests/rsrc/sample_annotation.bin");

    char *expected_annotation = "bin/audible -c -k DeadBeef";
    char buffer[ANNOTATION_MAX];

    int ret = read_annotation(buffer, strlen(expected_annotation));
    int exp_ret = 0;

    int string_compare_result = strncmp(expected_annotation, buffer, strlen(expected_annotation));

    cr_expect_eq(string_compare_result, 0, "Read annotations does not return the same annotation");
    cr_expect_eq(ret, exp_ret, "Invalid return for read annotation.  Got: %d | Expected: %d", ret, exp_ret);
}

Test(read_tests_suite, read_annotation_test_basic_1){
    open_input_file("./tests/rsrc/sample_annotation_1.bin");

    char * expected_annotation = "bin/audible -u";
    char buffer[ANNOTATION_MAX];

    int ret = read_annotation(buffer, strlen(expected_annotation));
    int exp_ret = 0;

    int string_compare_result = strncmp(expected_annotation, buffer, strlen(expected_annotation));

    cr_expect_eq(string_compare_result, 0, "Read annotations does not return the same annotation");
    cr_expect_eq(ret, exp_ret, "Invalid return for read annotation.  Got: %d | Expected: %d", ret, exp_ret);
}

Test(read_tests_suite, read_annotation_test_basic_2){
    open_input_file("./tests/rsrc/sample_annotation_2.bin");

    char * expected_annotation = "bin/audible -d -f 2";
    char buffer[ANNOTATION_MAX];

    int ret = read_annotation(buffer, strlen(expected_annotation));
    int exp_ret = 0;

    int string_compare_result = strncmp(expected_annotation, buffer, strlen(expected_annotation));

    cr_expect_eq(string_compare_result, 0, "Read annotations does not return the same annotation");
    cr_expect_eq(ret, exp_ret, "Invalid return for read annotation.  Got: %d | Expected: %d", ret, exp_ret);
}
////////////////////////////////////////////
// FRAME TESTS
////////////////////////////////////////////
Test(read_tests_suite, valid_read_frame) {
    int fd;
    char *filename = "./tests/rsrc/valid_read_frame.bin";
    int num_chan = 2;
    int bytes_per_sample = 4;

    if ((fd = open(filename, O_RDONLY)) < 0) {
        fprintf(stderr, "Couldn't open file: %s, ERRNO: %s", filename, strerror(errno));
        return;
    }

    if (dup2(fd, STDIN_FILENO) < 0) {
        fprintf(stderr, "Couldn't dup stdin to fd");
        return;
    }

    int fp[2];
    int fp_expected[] = {0, 0};
    int ret = read_frame(fp, num_chan, bytes_per_sample);
    int compare_result = 0;

    for (int i = 0; i < num_chan; i++)
    {
        if (fp_expected[i] != fp[i]){ 
            compare_result = 1;
            break;
        }
        fflush(stdout);
    }
    int exp_ret = 1;

    cr_expect_eq(compare_result, 0, "Read frames did not return the expected value");
    cr_expect_eq(ret, exp_ret, "Valid return for read header.  Got: %d | Expected: %d", ret, exp_ret);

    if (close(fd) < 0)
        fprintf(stderr, "Couldn't close file: %s", filename);
}

Test(read_tests_suite, valid_read_frame_1) {
    int fd;
    char *filename = "./tests/rsrc/valid_read_frame_1.bin";
    int num_chan = 2;
    int bytes_per_sample = 2;

    if ((fd = open(filename, O_RDONLY)) < 0) {
        fprintf(stderr, "Couldn't open file: %s, ERRNO: %s", filename, strerror(errno));
        return;
    }

    if (dup2(fd, STDIN_FILENO) < 0) {
        fprintf(stderr, "Couldn't dup stdin to fd");
        return;
    }

    int fp[1];
    int fp_expected[] = {0};
    int ret = read_frame(fp, num_chan, bytes_per_sample);
    int compare_result = 0;

    for (int i = 0; i < 1; i++)
    {
        if (fp_expected[i] != fp[i]){ 
            compare_result = 1;
            break;
        }
        fflush(stdout);
    }
    int exp_ret = 1;

    cr_expect_eq(compare_result, 0, "Read frames did not return the expected value");
    cr_expect_eq(ret, exp_ret, "Valid return for read header.  Got: %d | Expected: %d", ret, exp_ret);

    if (close(fd) < 0)
        fprintf(stderr, "Couldn't close file: %s", filename);
}
/*
Test(read_tests_suite, invalid_read_frame) {
    int fd;
    char *filename = "./tests/rsrc/invalid_read_frame.bin";
    int num_chan = -1;
    int bytes_per_sample = -2;

    if ((fd = open(filename, O_RDONLY)) < 0) {
        fprintf(stderr, "Couldn't open file: %s, ERRNO: %s", filename, strerror(errno));
        return;
    }

    if (dup2(fd, STDIN_FILENO) < 0) {
        fprintf(stderr, "Couldn't dup stdin to fd");
        return;
    }

    int fp[1];
    int ret = read_frame(fp, num_chan, bytes_per_sample);

    fflush(stdout);
    
    char exp_ret = 0;

    cr_expect_eq(ret, exp_ret, "Valid return for read header.  Got: %s | Expected: %s", ret, exp_ret);

    if (close(fd) < 0)
        fprintf(stderr, "Couldn't close file: %s", filename);
}
*/
