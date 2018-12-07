#include <criterion/criterion.h>
#include <criterion/logging.h>

#include "hw1.h"
#include "const.h"
#include "__write_helpers.h"

////////////////////////////////////////////
// HEADER TESTS
////////////////////////////////////////////
Test(write_tests_suite, write_header_test) {
    char *filename = "./tests/rsrc/written_header";
    open_output_file(filename);

    AUDIO_HEADER hp_written = {AUDIO_MAGIC, 32, 6, 4, 44100, 1};
    int ret = write_header(&hp_written);
    int exp_ret = 1;

    cr_expect_eq(ret, exp_ret, "Invalid return for write header.  Got: %d | Expected: %d", ret, exp_ret);

    fflush(stdout);
    open_input_file(filename);

    AUDIO_HEADER hp_read;
    int num_bytes_read = read(STDIN_FILENO, (void *)&hp_read, sizeof(AUDIO_HEADER));

    exp_ret = sizeof(AUDIO_HEADER);
    cr_expect_eq(num_bytes_read, exp_ret,
		 "It did not read the correct number of bytes from file.  Got: %d | Expected: %d",
		 num_bytes_read, exp_ret);

    reverse_bytes(&(hp_read.magic_number));
    cr_expect_eq(hp_read.magic_number, AUDIO_MAGIC,
		 "Magic number is not correct.  Got: %x | Expected: %x",
		 hp_read.magic_number, AUDIO_MAGIC);

    reverse_bytes(&(hp_read.data_offset));
    cr_expect_eq(hp_read.data_offset, hp_written.data_offset,
		 "Data offset is not correct.  Got: %d | Expected: %d",
		 hp_read.data_offset, hp_written.data_offset);

    reverse_bytes(&(hp_read.data_size));
    cr_expect_eq(hp_read.data_size, hp_written.data_size,
		 "Data size is not correct.  Got: %d | Expected: %d",
		 hp_read.data_size, hp_written.data_size);

    reverse_bytes(&(hp_read.encoding));
    cr_expect_eq(hp_read.encoding, hp_written.encoding,
		 "Encoding is not correct.  Got: %d | Expected: %d",
		 hp_read.encoding, hp_written.encoding);

    reverse_bytes(&(hp_read.sample_rate));
    cr_expect_eq(hp_read.sample_rate, hp_written.sample_rate,
		 "Sample rate is not correct.  Got: %d | Expected: %d",
		 hp_read.sample_rate, hp_written.sample_rate);

    reverse_bytes(&(hp_read.channels));
    cr_expect_eq(hp_read.channels, hp_written.channels,
		 "Channels is not correct.  Got: %d | Expected: %d",
		 hp_read.channels, hp_written.channels);
}

////////////////////////////////////////////
// ANNOTATION TESTS
////////////////////////////////////////////
Test(writer_tests_suite, write_annotation_test_basic){
    char *filename = "./tests/rsrc/write_annotation_test_basic";
    open_output_file(filename);

    char *annotation_buffer = "bin/audible -c -k DeadBeef";
    int ret = write_annotation(annotation_buffer, strlen(annotation_buffer));
    int exp_ret = 1;

    cr_expect_eq(ret, exp_ret, "Invalid return for write_annotation.  Got: %d | Expected: %d", ret, exp_ret);

    fflush(stdout);
    open_input_file(filename);

    char annotation_buffer_2[ANNOTATION_MAX];
    int num_bytes_read = read(STDIN_FILENO, (void *)&annotation_buffer_2, sizeof(annotation_buffer_2));

    cr_assert(num_bytes_read > 0, "An error occurred reading back the annotation.");

    int string_compare_result = strncmp(annotation_buffer, annotation_buffer_2, strlen(annotation_buffer));
    int string_compare_expected_result = 0;

    cr_expect_eq(string_compare_result, string_compare_expected_result,
		 "Annotations are not the same.  Got: '%s' | Expected: '%s'",
		 string_compare_result, string_compare_expected_result);
}

Test(writer_tests_suite, write_annotation_test_basic_1){
    char *filename = "./tests/rsrc/write_annotation_test_basic_1";
    open_output_file(filename);

    char *annotation_buffer = "bin/audible -u";
    int ret = write_annotation(annotation_buffer, strlen(annotation_buffer));
    int exp_ret = 1;

    cr_expect_eq(ret, exp_ret, "Invalid return for write_annotation.  Got: %d | Expected: %d", ret, exp_ret);

    fflush(stdout);
    open_input_file(filename);

    char annotation_buffer_2[ANNOTATION_MAX];
    int num_bytes_read = read(STDIN_FILENO, (void *)&annotation_buffer_2, sizeof(annotation_buffer_2));

    cr_assert(num_bytes_read > 0, "An error occurred reading back the annotation.");

    int string_compare_result = strncmp(annotation_buffer, annotation_buffer_2, strlen(annotation_buffer));
    int string_compare_expected_result = 0;

    cr_expect_eq(string_compare_result, string_compare_expected_result,
		 "Annotations are not the same.  Got: '%s' | Expected: '%s'",
		 string_compare_result, string_compare_expected_result);
}

Test(writer_tests_suite, write_annotation_test_basic_2){
    char *filename = "./tests/rsrc/write_annotation_test_basic_2";
    open_output_file(filename);

    char *annotation_buffer = "bin/audible -d -f 2";
    int ret = write_annotation(annotation_buffer, strlen(annotation_buffer));
    int exp_ret = 1;

    cr_expect_eq(ret, exp_ret, "Invalid return for write_annotation.  Got: %d | Expected: %d", ret, exp_ret);

    fflush(stdout);
    open_input_file(filename);

    char annotation_buffer_2[ANNOTATION_MAX];
    int num_bytes_read = read(STDIN_FILENO, (void *)&annotation_buffer_2, sizeof(annotation_buffer_2));

    cr_assert(num_bytes_read > 0, "An error occurred reading back the annotation.");

    int string_compare_result = strncmp(annotation_buffer, annotation_buffer_2, strlen(annotation_buffer));
    int string_compare_expected_result = 0;

    cr_expect_eq(string_compare_result, string_compare_expected_result,
		 "Annotations are not the same.  Got: '%s' | Expected: '%s'",
		 string_compare_result, string_compare_expected_result);
}

////////////////////////////////////////////
// FRAME TESTS
////////////////////////////////////////////
Test(writer_tests_suite, write_frame_test_basic){
    int fd;
    char *filename = "./tests/rsrc/write_frame_test_basic.bin";
    int num_chan = 2;
    int bytes_per_write = 4;

    if ((fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) {
        fprintf(stderr, "Couldn't open file: %s, ERRNO: %s\n", filename, strerror(errno));
        return;
    }

    if (dup2(fd, STDOUT_FILENO) < 0) {
        fprintf(stderr, "Couldn't dup stdout to fd\n");
        return;
    }

    int frame_buffer[] = {0, 0};
    int ret = write_frame(frame_buffer, num_chan, bytes_per_write);
    int exp_ret = 1;

    cr_expect_eq(ret, exp_ret, "Invalid return for write_frame.  Got: %d | Expected: %d", ret, exp_ret);

    if (fflush(stdout) != 0) {
        fprintf(stderr, "Error flushing stdout\n");
        close(fd);
        return;
    }

    if (lseek(fd, 0, SEEK_SET) < 0) {
        fprintf(stderr, "Error seeking to beginning of file\n");
        close(fd);
        return;
    }

    int num_bytes_read;
    int frame_buffer_2[2];
    if ((num_bytes_read = read(fd, (void *)&frame_buffer_2, sizeof(frame_buffer_2))) < 0) {
        fprintf(stderr, "Error occurred while reading file: %s\n", filename);
        close(fd);
        return;
    }

    int compare_res = 0, compare_exp_res = 0;
    for (int i = 0; i < 2; i++)
    {
        if (frame_buffer[i] != frame_buffer_2[i])
        {
            compare_res = 1;
            break;
        }
    }

    fflush(stdout);

    cr_expect_eq(compare_res, compare_exp_res, "Frames are not the same.  Got: '%s' | Expected: '%s'",compare_res, compare_exp_res);

    if (close(fd) < 0) fprintf(stderr, "Couldn't close file: %s\n", filename);
}

Test(writer_tests_suite, write_frame_test_basic_1){
    int fd;
    char *filename = "./tests/rsrc/write_frame_test_basic_1.bin";
    int num_chan = 2;
    int bytes_per_write = 2;

    if ((fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) {
        fprintf(stderr, "Couldn't open file: %s, ERRNO: %s\n", filename, strerror(errno));
        return;
    }

    if (dup2(fd, STDOUT_FILENO) < 0) {
        fprintf(stderr, "Couldn't dup stdout to fd\n");
        return;
    }

    int frame_buffer[] = {0, 0};
    int ret = write_frame(frame_buffer, num_chan, bytes_per_write);
    int exp_ret = 1;

    cr_expect_eq(ret, exp_ret, "Invalid return for write_frame.  Got: %d | Expected: %d", ret, exp_ret);

    if (fflush(stdout) != 0) {
        fprintf(stderr, "Error flushing stdout\n");
        close(fd);
        return;
    }

    if (lseek(fd, 0, SEEK_SET) < 0) {
        fprintf(stderr, "Error seeking to beginning of file\n");
        close(fd);
        return;
    }

    int num_bytes_read;
    int frame_buffer_2[1];
    if ((num_bytes_read = read(fd, (void *)&frame_buffer_2, sizeof(frame_buffer_2))) < 0) {
        fprintf(stderr, "Error occurred while reading file: %s\n", filename);
        close(fd);
        return;
    }

    int compare_res = 0, compare_exp_res = 0;
    for (int i = 0; i < 1; i++)
    {
        if (frame_buffer[i] != frame_buffer_2[i])
        {
            compare_res = 1;
            break;
        }
    }

    fflush(stdout);

    cr_expect_eq(compare_res, compare_exp_res, "Frames are not the same.  Got: '%s' | Expected: '%s'",compare_res, compare_exp_res);

    if (close(fd) < 0) fprintf(stderr, "Couldn't close file: %s\n", filename);
}
